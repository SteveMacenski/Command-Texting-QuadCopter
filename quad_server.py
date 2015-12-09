from flask import Flask, request, redirect
import sys
sys.path.append("C:\Python27\Lib\site-packages")
from flask import *
import twilio.twiml
import serial
import string
from time import sleep as sleep
from threading import Thread
import json
import os 
 
app = Flask(__name__)

@app.route("/", methods=['GET', 'POST'])
def sms():
                #ser = serial.Serial('COM6', 9600)       #com port for the xbee
    body = request.values.get('Body', None).lower()
    wordsList = body.split()
    print str(wordsList)
    sleep(2)
    for word in wordsList:
        commands = open('C:\Python27\Command-Texting-QuadCopter\AE483GroundStation\commands.txt','w')
        if "up" in word:
                #     ser.write('0')
            commands.write('U')
            sleep(1)
        elif "down" in word:
                #     ser.write('1')
            commands.write('D')
            sleep(1)
        elif "left" in word:
                #     ser.write('2')
            commands.write('L')
            sleep(1)
        elif "right" in word:
                #     ser.write('3')
            commands.write('R')
            sleep(1)
        elif "forward" in word:
                #     ser.write('4')
            commands.write('F')
            sleep(1)
        elif "back" in word:
                #     ser.write('5')
            commands.write('B')
            sleep(1)
        elif "land" in word:
            commands.write('Z')
                #     ser.write('6')
            resp = twilio.twiml.Response()
            resp.message("Landed!")
            return str(resp)
            sleep(1)
        elif "clean" in word:
            commands.write('C')
                #     ser.write('7')
            resp = twilio.twiml.Response()
            resp.message("Cleaned!")
            return str(resp)
            sleep(1)
        elif "gps" in word:
            commands.write('G')
                #     ser.write('8')
            resp = twilio.twiml.Response()
            resp.message("Moved to those coordinates!")
            return str(resp)
            sleep(1)
        else:
            # Bad command
            resp = twilio.twiml.Response()
            resp.message("Oops! Try saying up, down, left, right, forward, back, clean, land or GPS :)")
            return str(resp)
        commands.close()    #ensures last commands is the only one taken
        
    # Good command
    resp = twilio.twiml.Response()
    if len(wordsList) > 1:
        resp.message("Moved around!")
    elif len(wordsList) == 1:
        resp.message("Moved %s!" % body.strip())
    return str(resp)


if __name__ == "__main__":
    app.run(debug=True)
