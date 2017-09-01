Image2Code is a small java utility to convert images into a byte array that can be used as a bitmap in the Adafruit-GFX-library

# USER Guide

## To Run 
Run the command: ./gradlew run to launch the application. 

## Using the Application 
1. Click "choose file" button. 
2. Then copy paste the generated code in your arduino sketch. 

## Dependencies: 
1. Java: If you do not have this installed, it can be found at: http://www.java.com/en/download/ if you havn't already done so.
2. Gradle: The gradlew will take care of this dependency. Just running any gradlew command will pull down the dependency

#DEV guide:
This sub-project using Gradle to build. To build a runnable jar: ./gradlew jar 
