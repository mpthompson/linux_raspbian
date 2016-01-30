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

static int snd_rpi_nuvoton_mono_init(struct snd_soc_pcm_runtime *rtd)
{
	return 0;
}

static int snd_rpi_nuvoton_mono_hw_params(struct snd_pcm_substream *substream,
				       struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;

	unsigned int sample_bits =
		snd_pcm_format_physical_width(params_format(params));

	return snd_soc_dai_set_bclk_ratio(cpu_dai, sample_bits * 2);
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
