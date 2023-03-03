# SIF
Stands for Simulated Input Framework. It contains random number generator which utilizes your empty pin, uses arithmetic progression and sinusoids to prevent heuristic analysis, has autonomous coordinates system and doesn't use serial port. I also tried to make the main running part of the code somewhat simple, so even someone unfamiliar with programming could edit and use it, and explained most of the core code lines so it can be edited to your heart's content.

# Preparations
I expect you to have Arduino Leonardo and USB Host Shield on it. Optionally mouse Ritmix RMW-555 and PC running Windows 10 x64 would be good but not necessary.

Your A5 pin should be empty for randomizer to work correctly.

Then you need to change core arduino file USBDesc.h to disable serial connection, in windows Arduino IDE 2.0.3 it's usually located at: c:\Users\\%username%\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\cores\arduino\
Either replace it with mine USBDesc.h if your IDE version is the same, or better manually uncomment //#define CDC_DISABLED

# Usage
As a demonstration by default SIF does a simple sequence: after you make a click it moves your cursor to a specific point then makes a click. Scrolling your mouse wheel down makes mouse function normally and scrolling your mouse wheel up enables script again.

Unrandomized destination:
https://user-images.githubusercontent.com/34285977/222722816-8c4493c1-072b-4ac5-905c-44b7010ed2ce.mp4

Randomized:

If your screen's resolution isn't 1920x1080 you should change variables accordingly: "wdt" is screen's width and "hgt" is screen's height.

If your mouse differs from mine you need to test it's polling rate. There's [MouseSpeedTester2](/MouseSpeedTester2/) folder in repository with a programm for Windows and an instruction how to do it, use it and change your "mic" variable accordingly. Also it seems that with some Logitech mouses SIF doesn't work at all, I have one that it works with and one that doesn't. Expect that your mouse might not be compatible. I'm currently working on it.

The safest way to start your Arduino undetected is to connect it before turning your computer on, but otherwise it doesn't seem to make serial connections except when resetting so you should be good overall. Also don't connect another mouse to PC while arduino is still plugged in to avoid detection.

Since your Arduino is hidden from your PC you should use reset to write different sketches, here's some ways how to do it: https://stackoverflow.com/questions/5290428/how-can-i-reset-an-arduino-board

If you ever show this to someone tell them I made it and you love it
