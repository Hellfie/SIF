# SIF
Simulated Input Framework

Your A5 pin should be empty for randomizer to work correctly

Then you need to change core arduino file USBDesc.h to disable serial connection, in windows Arduino IDE it's usually located at: c:\Users\%username%\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\cores\arduino\
Either replace it with mine if your IDE version is the same, or better manually uncomment //#define CDC_DISABLED

The safest way to start your Arduino undetected is to connect it before turning your computer on. I'm pretty sure it won't fool Vanguard though, for that you may try connecting battery to your Arduino so it's already turned on before computer boot, that might work.

Since your Arduino is hidden from your PC you should use reset to write different sketches, here's some ways how to do it: https://stackoverflow.com/questions/5290428/how-can-i-reset-an-arduino-board

If you ever show this to someone tell them I made it
