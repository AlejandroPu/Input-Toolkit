### Useful notes about connecting two Raspberry Pi using one as Access Point(AP)

This `.txt` is **just my personal logbook** from when I tried to connect
two Raspberry Pi boards (one acting as an access point) and stream video
with `ffmpeg`.  
I pasted commands and tweaks as I learned; **most of it never worked**
or I’ve forgotten what it was for.

- **What eventually did work**  
  - Booting the Pi as an **access point** (AP) with `hostapd` + `dnsmasq`.  
  - Capturing and sending video with **`ffmpeg`** (the lines at the end of the file).  
- **Outcome**: These experiments helped me pivot the product strategy during Startup School.

- **What I could NOT get running**  
  - Pure Wi-Fi Direct setup with `wpa_supplicant` (section “Create GO”).  
  - Several advanced `ffmpeg` flags noted as trials.

Current purpose: **historical reference only.**  
Don’t copy-paste everything—take only the bits you need and adapt them
to your environment.
