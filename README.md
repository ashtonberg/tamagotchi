# Ashton's Knockoff Tamagotchi

This is my final prject for CSCI 5143, Real-Time and Embedded Systems, Spring 2024.

For my project, I created a simpler version of a Tamagotchi, which is a digital pet on a keychain. The "5143 Final Report.pdf" file is a detailed report of the project that explains how everything actually works. If you want to see some pictures and read a description of the final product, check out the "Final Product" section towards the end of this pdf. Additionally, there are some videos and a picture in the "media" folder. The descriptions of the videos are below.

## Videos
### cursor.mov
This video shows the movement of the cursor on the home screen using the green button. Pressing the button moves the cursor to the right. If the button is pressed when the cursor is on the fourth option, it will be removed from the screen. If then pressed again, it will wrap around to the first option.

### feed.mov
This video shows me feeding the pet. First, I check the stats page. Then, I go and feed the pet a couple of times. Checking the stats page again shows that their hunger bar has gone up. Feeding the pet when they are hungry will make a heart appear, but once they are full, an 'X' shows up instead.

### light.mov
This video shows the toggling of the pet's light. When the light is on, the light on the right side appears lit up, and when it's off, it looks like it's turned off. The pet likes having its light on, so if it's off for too long their happiness will start dropping. However, the pet will fall asleep every couple of mintues, and you need to turn their light off before they wake up otherwise their happiness will drop. 

### game.mov
This video shows the minigame you can play with your pet to increase their happiness. Selecting the 'game' option will enter you into a minigame and a song (lostwoods from Zelda) will begin playing. The game plays for three rounds, and the current round is shown at the top. The goal is to guess which direction the pet will turn. The green button will guesses left, and the blue button guesses right. Once you make your selection, the pet will choose a random direction to turn. If they turn in the direction you guessed, a heart will pop up. If they turn in the opposite direction, an 'X' will pop up. After playing for three rounds, the pet's happiness will go up based on how many guesses you got correct, you will return to the home screen, and the music will turn off.


## Extra Notes
## Yellow Button and Potentiometer
There is a third button on the breadboard, as well as a potentiometer dial. Turning the dial on its own won't do anything. If you press and hold the yellow button and then turn the dial, it will adjust the brightness of the display. Releasing the button will make it so the potentiometer doesn't do anything again. I tried taking a video of this, but it didn't show up that well on camera. 

## Bluetooth
You can connect to the device over bluetooth. I used the LightBlue app for communicating with it. While connected, if you send a hex value ending in 0xAB, it's supposed to simulate "petting" your pet, and a heart will pop up in the bottom left of the screen. Sending any other value won't do anything. If the pet's happiness or hunger drop below half while connected to a device, then it will send a notification to your device. 



