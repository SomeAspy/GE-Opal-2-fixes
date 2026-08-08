# GE-Opal-2-fixes
This repository documents, all my attempts to make the GE Opal 2 not kill itself
(More specifically, the *GE Profile:tm: Opal:tm: 2.0 Ultra Nugget Ice Maker )

Important context to a lot of frustration here: This is a $500 dollar ice machine. It has a lifespan of 6 months to 1 year

> [!WARNING]  
> Schematics and code are not final or tested yet.

### My blog post on this

[A Scathing Review of the GE Opal 2 Nugget Ice Maker](https://blog.aspy.dev/a-scathing-review-of-the-ge-opal-2-nugget-ice-maker/)

# Background

I've tried putting lubrication on the auger ([./squeakFromShaft.md](./squeakFromShaft.md)) but it did not resolve the squeaking issue
I've decided I am going to replace the PCB with an arduino to implement a proper defrost cycle.

## Notice

As I was writing this, Reddit decided to delete the 2 year old post I was using as a reference.
Some of the more important images are available in ./images
https://web.archive.org/web/20250804095737/https://www.reddit.com/r/IceChewersAnonymous/comments/1hxlbkg/fixing_the_opal_20/

## The Problem

The GE Opal ice maker has well-known issues where it will start to fail after about a year of use, and in some cases, prior.
The machine is incredibly high maintenance, [with GE recommending running bleach through the machine](https://products.geappliances.com/appliance/gea-support-search-content?contentId=000060634) ([archive.org capture](https://web.archive.org/web/20260716162758/https://products.geappliances.com/appliance/gea-support-search-content?contentId=000060634))

There is even (as of writing) [an ongoing class action](https://classlawdc.com/2026/01/21/ge-profile-opalnugget-ice-maker-series-1-0-and-2-0-defective-product-investigation/)

These machines do not fail gracefully, often making screeching and whining noises due to the internal parts getting frozen and locked up. This commonly involves the destruction of the gearbox, auger, and/or bearings.

Browsing Reddit reveals many owners have constant issues with these machines, with one redditor even claiming to go through 4 machines in 2 years. Despite its flaws, this machine is well liked by r/IceChewersAnonymous, though they also agree that it has significant longevity issues. [A quick search shows the love/hate the community has for it.](https://www.reddit.com/r/IceChewersAnonymous/search/?q=opal+2)

More importantly, several members of the community have taken attempts to repair the machine pretty far.
- https://www.reddit.com/r/IceChewersAnonymous/comments/1ophadu/opal_20_repair_update/
- https://www.reddit.com/r/IceChewersAnonymous/comments/1hxlbkg/fixing_the_opal_20/
- https://www.reddit.com/r/IceChewersAnonymous/comments/159ts6e/ge_opal_20_add_water_fix/

# Piping

![Piping](./pipes.svg)

# Nominal voltages recorded from the opal 2 ice maker during operation

| Part                    | Connector Type          | Voltage
|------------------------ |------------------------ |-------- |
| UV Light                | JST XH 2-pin connector  | 12V DC  |
| Compressor              | JST VHR 3-pin connector | 120V AC |
| Auger Motor             | JST VHR 3-pin connector | 120V AC |
| Pump                    | JST XH 2-pin connector  | 12V DC  |
| Fan                     | JST XH 2-pin connector  | 12V DC  |
| WiFi Board              | TBD                     | TBD     |
| Front Panel             | TBD                     | TBD     |
| Ice Box LED             | JST XH 2-pin connector  | 12V DC  |
| Ice box presence switch | JST XH 2-pin connector  | 5V DC   |
| Internal Tank Floats    | JST XH 4-pin connector  | 5V DC   |
| IR LED For Capacity     | JST XH 2-pin connector  | 5V DC   |
| IR Receiver             | JST XH 2-pin connector  | 5V DC   |
| AC Input                | JST VHR 3-pin connector | 120V AC |

- Internal Tank Floats (4 wires, 2 floats)
  - Black & Red: Lower float (float low = closed circuit)
  - Yellow & White: Upper float (float high = closed circuit)
