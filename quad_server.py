from flask import Flask, request, redirect
from flask import *
import twilio.twiml
import serial
import string
from time import sleep as sleep
from threading import Thread
import json
import os 
 
app = Flask(__name__)
 
                #@app.route("/", methods=['GET', 'POST'])
                #def hello_monkey():
                #    """Respond to incoming texts with a simple text message."""
 
                #    resp = twilio.twiml.Response()
                #    resp.message("Hello, please send your command")
                #    return str(resp)
 
                ##matlab_command = open('commands.txt','w')   #proof for each write over it replaces text. 
                ##matlab_command.write('initialized');
                ##matlab_command.close()
                ##matlab_command = open('commands.txt','w')
                ##matlab_command.write('try two');
                ##matlab_command.close()
                ##matlab_command = open('commands.txt','w')
                ##matlab_command.write('try three');
                ##matlab_command.close()

@app.route("/", methods=['GET', 'POST'])
def sms():
                #ser = serial.Serial('COM6', 9600)       #com port for the xbee
    body = request.values.get('Body', None).lower()
    wordsList = body.split()
    print str(wordsList)
    sleep(2)
    for word in wordsList:
        commands = open('commands.txt','w')
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
        elif "paint" in word:
            commands.write('C')
                #     ser.write('4')
            resp = twilio.twiml.Response()
            resp.message("Cleaned!")
            return str(resp)
            sleep(1)
        elif "GPS" in word:
            commands.write('G')
                #     ser.write('5')
            resp = twilio.twiml.Response()
            resp.message("Went to GPS coordinates!")
            return str(resp)
            sleep(1)
        else:
            # Bad command
            resp = twilio.twiml.Response()
            resp.message("Oops! Try saying up, down, left, right, clean, or GPS :)")
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
