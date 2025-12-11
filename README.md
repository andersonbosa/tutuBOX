<div align="center">
  <img src="https://github.com/user-attachments/assets/0eba90bc-2ff5-40df-88a1-92e23396d1d3" alt="logo" width="100" height="auto" />
  
  <h1>tutuBOX</h1>
  <p>All-in-One Gadget for BLE and 2.4GHz Networks</p>
  <p>By TUTU Devices | Maintained by andersonbosa</p>

  <!-- Badges -->
  <p>
    <a href="https://github.com/andersonbosa/tutuBOX" title="GitHub repo">
      <img src="https://img.shields.io/static/v1?label=tutuBOX&message=andersonbosa&color=purple&logo=github" alt="tutuBOX - andersonbosa">
    </a>
    <a href="https://github.com/andersonbosa/tutuBOX">
      <img src="https://img.shields.io/github/stars/andersonbosa/tutuBOX?style=social" alt="stars - tutuBOX">
    </a>
    <a href="https://github.com/andersonbosa/tutuBOX">
      <img src="https://img.shields.io/github/forks/andersonbosa/tutuBOX?style=social" alt="forks - tutuBOX">
    </a>
  </p>

  <h3>
    <a href="https://andersonbosa.github.io/tutuBOX">🌐 Learn More</a> ·
    <a href="https://discord.gg/DISCORD_ID">💬 Join Discord</a>
  </h3>
</div>

---

## What is tutuBOX?

**tutuBOX** is your pocket-sized 2.4GHz wireless lab. Think of it as a swiss army knife for the entire 2.4GHz spectrum - Bluetooth, BLE, WiFi, and everything in between. Perfect for security researchers, pentesters, hackers, and curious tinkerers who want to understand how wireless protocols really work.

Built around an ESP32 with triple NRF24 modules, a crisp OLED display, and a 2500mAh rechargeable battery, tutuBOX lets you explore the invisible world of 2.4GHz radio anywhere - no cables needed. Scan for hidden Bluetooth devices, detect AirTags tracking you, find credit card skimmers, analyze RF signals, test wireless security, and way more.

**→ [Check out all the features at andersonbosa.github.io/tutuBOX](https://andersonbosa.github.io/tutuBOX)**

<div align="center">
  <img src="https://github.com/user-attachments/assets/530e5686-09db-4f02-aabe-80a8abcbb036" alt="tutuBOX Interface" width="650" />
</div>

---

## ⚡ Why You'll Love It

- **Plug & Play** – USB-C powered, works right out of the box
- **All-Day Battery** – 2500mAh battery provides up to a full day of portable use
- **Level Up System** – Built-in RPG mechanics track your progress as you explore wireless protocols
- **Open Source** – Fully customizable firmware with active community development
- **Complete 2.4GHz Toolkit** – 20+ built-in features for Bluetooth, BLE, WiFi, and RF analysis
- **Pocket-Sized** – Take it anywhere, scan everything
- **Active Updates** – New features added regularly by the community

**Ready to dive in? [Purchase tutuBOX at shop.andersonbosa.github.io/tutuBOX](https://shop.andersonbosa.github.io/tutuBOX)**

---

## 🎯 What Can It Do?

> **⚠️ Note:** Additional advanced tools can be enabled in the Settings menu.

### 📶 WiFi Tools
- **WiFi Scanner** – Detects nearby WiFi access points
- **Channel Analyzer** – Monitors WiFi channel utilization and signal strength for network planning
- **WiFi Deauther** – Educational tool for testing network security with deauthentication frames on authorized networks
- **Deauth Scanner** – Monitors and analyzes WiFi deauthentication frames in real-time
- **Beacon Spam** – Broadcasts multiple fake WiFi networks for testing. Choose to clone real nearby networks, select specific SSIDs, or use a list of random names.
- **Evil Portal** – Creates captive portal with multiple templates (Generic, Facebook, Google) that automatically scans nearby networks for realistic SSID spoofing and credential capture.
- **Pineapple Detector** – Detect and identify nearby Pineapple devices
- **Pwnagotchi Detector** – Detects nearby Pwnagotchi devices and displays their information
- **Pwnagotchi Spam** - Pwnagotchi grid flooding tool that generates fake beacon frames with randomized identities, faces, names, and versions (contains optional DoS mode).

### 🔵 Bluetooth (BLE) Tools
- **BLE Scanner** – Detects nearby BLE devices
- **tutuBOX Detector** – Discovers nearby tutuBOX devices and displays their information including level, version, and signal strength.
- **Flipper Scanner** – Detects nearby Flipper Zero devices
- **Axon Detector** – Detects nearby Axon devices (body cameras, tasers, and other law enforcement equipment)
- **Meshtastic Detector** - Detects nearby devices running Meshtastic firmware
- **MeshCore Detector** - Detects nearby devices running MeshCore firmware
- **Skimmer Detector** – Detects HC-03, HC-05, and HC-06 Bluetooth modules commonly used in credit card skimming devices.
- **AirTag Detector** – Scans for and identifies nearby Apple AirTag devices.
- **AirTag Spoofer** – Clones and rebroadcasts detected Apple AirTag devices for selective or bulk spoofing.
- **Tile Detector** - Scans for and identifies nearby Tile Tracker devices.
- **BLE Spammer** – Broadcasts BLE advertisement packets for testing
- **Swift Pair** - Triggers Windows Swift Pair notifications by broadcasting fake Microsoft device advertisements.
- **Sour Apple** – Mimics Apple Bluetooth signals like AirPods pairing pop-up to test device resilience against protocol exploits.
- **BLE Spoofer** – Clones and rebroadcasts detected BLE devices with complete 1:1 replication of MAC address, name, advertising data, scan response, and connectable state.

### 📡 Signal & Protocol Tools
- **Flock Detector** - Detects Flock Safety surveillance cameras using dual-mode WiFi and BLE scanning. Identifies devices through SSID patterns, MAC OUI prefixes, and Bluetooth device names. Features real-time signal strength tracking with detailed device info and a locate mode for pinpointing camera positions.
- **Scanner** – Scans the 2.4GHz frequency band to detect active channels and devices
- **Analyzer** – Analyzes detected signals and provides detailed activity information

### 🎮 Leveling System
tutuBOX features a built-in RPG-style leveling system that tracks your usage throughout using the device:

- **Level Progression** – Gain XP by using different tools and features
- **Rank System** – Progress through 9 different ranks
- **Usage Tracking** – Different XP rates for scanning, attacks, and utilities/misc
- **Session Bonuses** – Extra XP for extended tool usage
- **Level Display** – Current level shown on main menu, detailed stats accessible via RIGHT arrow
- **Progress Persistence** – Level data saved to EEPROM, survives power cycles
- **XP Reset** – Reset progress via Settings menu if desired
- **Device Networking** – Your level and version are automatically broadcasted to nearby tutuBOX devices for discovery

Hit RIGHT in the main menu to check your stats. Level up by tinkering with RF signals and unlock ranks as you progress. Other tutuBOX users can see your progress when they scan for nearby devices!

---

## 🛠️ Hardware Specs

| Component  | Details                                  |
|-----------:|------------------------------------------|
| Microcontroller      | ESP32 WROOM‑32U (dual‑core, Wi‑Fi + BT)  |
| Wireless Modules     | 3× NRF24 GTmini modules        |
| Display    | 0.96" OLED                               |
| Power      | USB‑C + 2500mAh rechargeable battery     |
| Battery    | Up to a full day typical use             |
| Case       | Protective enclosure included            |
| Debug      | UART                                     |

Get yours: https://shop.andersonbosa.github.io/tutuBOX

---

## 🚀 Getting Started

### First Time Setup

Purchase a tutuBOX from **[shop.andersonbosa.github.io/tutuBOX](https://shop.andersonbosa.github.io/tutuBOX)** and flash the firmware in minutes using our web-based flasher!

### Firmware Installation & Updates

Get up and running or update to the latest features:

#### Easy Mode - Web Flasher (Recommended)
1. Head to **[andersonbosa.github.io/tutuBOX/flasher](https://andersonbosa.github.io/tutuBOX/flasher)**
2. Plug in your tutuBOX via USB-C
3. Click **Install tutuBOX Firmware**
4. Done!

#### Advanced - PlatformIO
For developers who want to build from source or customize the firmware:

1. Install [VS Code](https://code.visualstudio.com/) and [PlatformIO](https://platformio.org/install/ide?install=vscode)
2. Clone or download this repo
3. Open the folder in VS Code
4. Hit Upload in PlatformIO
5. Flash complete!

**Troubleshooting:**
- Can't find the port? Install [CP210x drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
- Upload failing? Hold the BOOT button while flashing
- Still stuck? Hit up our [Discord](https://discord.gg/DISCORD_ID) - we're here to help!

---

## ⚠️ Legal Disclaimer

**Use tutuBOX responsibly.** It's built for education, security research, and authorized testing only.

- Don’t attack networks you don’t own
- Always obtain permission before testing
- Know your local laws; some features may be restricted
- You’re responsible for how you use it

By using tutuBOX, you agree to use it ethically and legally. We're not liable for misuse.

---

## ❓ FAQ

**Is tutuBOX legal to own?**  
Yes, but some features may be restricted depending on your location and use. Follow local laws and obtain permission.

**How long does the battery last?**  
Up to a full day of typical use. Heavy continuous scanning may reduce runtime; lighter intermittent use can extend it.

**Can I develop my own tools?**  
Yes. The firmware is open source. You can customize and add features. Join our Discord if you need help.

**Does it come with firmware pre‑installed?**  
tutuBOX ships ready to flash. Use the web flasher at https://andersonbosa.github.io/tutuBOX/flasher to get running in minutes.

---

## 💬 Join the Community

Got questions? Want to show off your device? Need help?

- **[Discord](https://discord.gg/DISCORD_ID)** - Most active community spot
- **[GitHub Issues](https://github.com/andersonbosa/tutuBOX/issues)** - Report bugs or request features
- **[andersonbosa.github.io/tutuBOX](https://andersonbosa.github.io/tutuBOX)** - Full docs and guides

---

## 💝 Support the Project

Love tutuBOX? Here's how you can help:

- ⭐ Star this repo
- 🛒 **[Buy tutuBOX at shop.andersonbosa.github.io/tutuBOX](https://shop.andersonbosa.github.io/tutuBOX)**
- ☕ Buy us a coffee:

### ReBuilt By
- [andersonbosa](https://github.com/andersonbosa)

---

## 🙏 Thanks To

- [Original nyanBOX Project](https://github.com/jbohack/nyanBOX)
- [Original nRFBOX Project](https://github.com/cifertech/nrfbox)
- [Poor Man's 2.4 GHz Scanner](https://forum.arduino.cc/t/poor-mans-2-4-ghz-scanner/54846)
- [arduino_oled_menu](https://github.com/upiir/arduino_oled_menu)
- [Universal-RC-system](https://github.com/alexbeliaev/Universal-RC-system)
- [AppleJuice](https://github.com/ECTO-1A/AppleJuice)
- [ESP32-Sour-Apple](https://github.com/RapierXbox/ESP32-Sour-Apple)
- [PwnGridSpam](https://github.com/7h30th3r0n3/PwnGridSpam)
- [ESP32-AirTag-Scanner](https://github.com/MatthewKuKanich/ESP32-AirTag-Scanner)
- [ESP Web Tools](https://esphome.github.io/esp-web-tools/)
- [Flock You](https://github.com/colonelpanichacks/flock-you)

And thanks to everyone who's contributed code, reported bugs, purchased a device, or just shared the love. You're awesome!

---

## 📜 License

MIT License - see [LICENSE](LICENSE) for details.

---

<div align="center">
  <h3>Ready to explore the 2.4GHz spectrum?</h3>
  <p>
    <a href="https://shop.andersonbosa.github.io/tutuBOX"><strong>🛒 Buy tutuBOX Now</strong></a>
  </p>
  <p>#BadgeLife</p>
</div>