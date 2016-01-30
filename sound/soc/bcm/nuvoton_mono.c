/*
 * ASoC Driver for Nuvoton Mono DAC
 *
 * Author:	Mike Thompson <mpthompson@gmail.com>
 *		Copyright 2015
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/module.h>
#include <linux/platform_device.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/jack.h>

#include "../codecs/wm8510.h"

static const struct snd_soc_dapm_widget nuvoton_mono_dapm_widgets[] = {
	SND_SOC_DAPM_MIC("Int Mic", NULL),
	SND_SOC_DAPM_SPK("Ext Spk", NULL),
};

static const struct snd_soc_dapm_route nuvoton_mono_audio_map[] = {
	/* speaker connected to SPKOUT */
	{"Ext Spk", NULL, "SPKOUTP"},
	{"Ext Spk", NULL, "SPKOUTN"},
	{"Mic Bias", NULL, "Int Mic"},
	{"MICN", NULL, "Mic Bias"},
	{"MICP", NULL, "Mic Bias"},
};

static int snd_rpi_nuvoton_mono_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_dapm_context *dapm = &rtd->card->dapm;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;

	/* Add DAPM widgets */
	snd_soc_dapm_new_controls(dapm, nuvoton_mono_dapm_widgets, 
					ARRAY_SIZE(nuvoton_mono_dapm_widgets));

	/* Setup audio path interconnects */
	snd_soc_dapm_add_routes(dapm, nuvoton_mono_audio_map, 
					ARRAY_SIZE(nuvoton_mono_audio_map));

	/* Always connected pins */
	snd_soc_dapm_enable_pin(dapm, "Int Mic");
	snd_soc_dapm_enable_pin(dapm, "Ext Spk");
	snd_soc_dapm_sync(dapm);

	/* Make CSB show PLL rate */
	snd_soc_dai_set_clkdiv(codec_dai, WM8510_OPCLKDIV,
					WM8510_OPCLKDIV_1 | 4);

	return 0;
}

static int snd_rpi_nuvoton_mono_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	unsigned int pll_out = 0, bclk = 0, mclk_div = 0;
	int ret;

	/* Because the Raspberry Pi doesn't produce MCLK, we're going to 
	 * let the CODEC be in charge of all the clocks 
	 */
	const unsigned int fmt = (SND_SOC_DAIFMT_I2S |
				  SND_SOC_DAIFMT_NB_NF |
				  SND_SOC_DAIFMT_CBM_CFM);

	/* Figure out PLL and BCLK dividers for WM8510 */
	switch (params_rate(params)) {
	case 48000:
		pll_out = 24576000;
		mclk_div = WM8510_MCLKDIV_2;
		bclk = WM8510_BCLKDIV_8;
		break;

	case 44100:
		pll_out = 22579200;
		mclk_div = WM8510_MCLKDIV_2;
		bclk = WM8510_BCLKDIV_8;
		break;

	case 22050:
		pll_out = 22579200;
		mclk_div = WM8510_MCLKDIV_4;
		bclk = WM8510_BCLKDIV_8;
		break;

	case 16000:
		pll_out = 24576000;
		mclk_div = WM8510_MCLKDIV_6;
		bclk = WM8510_BCLKDIV_8;
		break;

	case 11025:
		pll_out = 22579200;
		mclk_div = WM8510_MCLKDIV_8;
		bclk = WM8510_BCLKDIV_8;
		break;

	case 8000:
		pll_out = 24576000;
		mclk_div = WM8510_MCLKDIV_12;
		bclk = WM8510_BCLKDIV_8;
		break;

	default:
		pr_warning("nuvoton_mono: Unsupported sample rate %d\n",
			   params_rate(params));
		return -EINVAL;
	}

	/* Set CPU and CODEC DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, fmt);
	if (ret < 0) {
		pr_warning("nuvoton_mono: "
			   "Failed to set CODEC DAI format (%d)\n",
			   ret);
		return ret;
	}
	ret = snd_soc_dai_set_fmt(cpu_dai, fmt);
	if (ret < 0) {
		pr_warning("nuvoton_mono: "
			   "Failed to set CPU DAI format (%d)\n",
			   ret);
		return ret;
	}

	/* Configure CODEC clocks */
	pr_debug("nuvoton_mono: "
		 "pll_in = %ld, pll_out = %u, bclk = %x, mclk = %x\n",
		 12000000l, pll_out, bclk, mclk_div);

	ret = snd_soc_dai_set_clkdiv(codec_dai, WM8510_BCLKDIV, bclk);
	if (ret < 0) {
		pr_warning
		    ("nuvoton_mono: Failed to set CODEC DAI BCLKDIV (%d)\n",
		     ret);
		return ret;
	}

	ret = snd_soc_dai_set_pll(codec_dai, 0, 0,
					12000000, pll_out);
	if (ret < 0) {
		pr_warning("nuvoton_mono: Failed to set CODEC DAI PLL (%d)\n",
			   ret);
		return ret;
	}

	ret = snd_soc_dai_set_clkdiv(codec_dai, WM8510_MCLKDIV, mclk_div);
	if (ret < 0) {
		pr_warning("nuvoton_mono: Failed to set CODEC MCLKDIV (%d)\n",
			   ret);
		return ret;
	}

	return 0;
}

/* machine stream operations */
static struct snd_soc_ops snd_rpi_nuvoton_mono_ops = {
	.hw_params = snd_rpi_nuvoton_mono_hw_params,
};

static struct snd_soc_dai_link snd_rpi_nuvoton_mono_dai[] = {
{
	.name		= "Nuvoton Mono",
	.stream_name	= "Nuvoton Mono DAC",
	.cpu_dai_name	= "bcm2708-i2s.0",
	.codec_dai_name	= "wm8510-hifi",
	.platform_name	= "bcm2708-i2s.0",
	.codec_name	= "wm8510.1-001a",
	.dai_fmt	= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
				SND_SOC_DAIFMT_CBS_CFS,
	.ops		= &snd_rpi_nuvoton_mono_ops,
	.init		= snd_rpi_nuvoton_mono_init,
},
};

/* audio machine driver */
static struct snd_soc_card snd_rpi_nuvoton_mono = {
	.name         = "NuvotonMonoDAC",
	.dai_link     = snd_rpi_nuvoton_mono_dai,
	.num_links    = ARRAY_SIZE(snd_rpi_nuvoton_mono_dai),
};

static int snd_rpi_nuvoton_mono_probe(struct platform_device *pdev)
{
	int ret = 0;

	snd_rpi_nuvoton_mono.dev = &pdev->dev;

	if (pdev->dev.of_node) {
		struct device_node *i2s_node;
		struct snd_soc_dai_link *dai = &snd_rpi_nuvoton_mono_dai[0];
		i2s_node = of_parse_phandle(pdev->dev.of_node,
					"i2s-controller", 0);

		if (i2s_node) {
			dai->cpu_dai_name = NULL;
			dai->cpu_of_node = i2s_node;
			dai->platform_name = NULL;
			dai->platform_of_node = i2s_node;
		}
	}

	ret = snd_soc_register_card(&snd_rpi_nuvoton_mono);
	if (ret)
		dev_err(&pdev->dev, "snd_soc_register_card() failed: %d\n", ret);

	return ret;
}

static int snd_rpi_nuvoton_mono_remove(struct platform_device *pdev)
{
	return snd_soc_unregister_card(&snd_rpi_nuvoton_mono);
}

static const struct of_device_id snd_rpi_nuvoton_mono_of_match[] = {
	{ .compatible = "nuvoton,nuvoton-mono", },
	{},
};
MODULE_DEVICE_TABLE(of, snd_rpi_nuvoton_mono_of_match);

static struct platform_driver snd_rpi_nuvoton_mono_driver = {
	.driver = {
		.name = "snd-rpi-nuvoton-mono",
		.owner = THIS_MODULE,
		.of_match_table = snd_rpi_nuvoton_mono_of_match,
	},
	.probe = snd_rpi_nuvoton_mono_probe,
	.remove = snd_rpi_nuvoton_mono_remove,
};

module_platform_driver(snd_rpi_nuvoton_mono_driver);

MODULE_AUTHOR("Mike Thompson <mpthompson@gmail.com>");
MODULE_DESCRIPTION("ASoC Driver for Nuvoton Mono DAC");
MODULE_LICENSE("GPL v2");
