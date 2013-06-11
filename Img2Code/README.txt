Image2Code is a small java utility to convert images into a byte array that can be used as a bitmap in the Adafruit-GFX-library

USER guide:
___________
just double click Image2Code.jar and click "choose file" button. Then copy paste the generated code in your arduino sketch. 

Note that this is a java-based program and you might need to install java from http://www.java.com/en/download/ if you havn't already done so.


DEV guide:
__________
ant file (buildImage2Code.xml) included to build the jar file from the source  if people are willing to tweak the code ;-).Just issue "ant -f buildImage2Code.xml" from a command prompt.Ant can be downloaded at http://ant.apache.org/


