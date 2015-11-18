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
 

    
@app.route("/", methods=['GET', 'POST'])
def sms():
    #ser = serial.Serial('COM6', 9600)       #com port for the xbee
    body = request.values.get('Body', None).lower()
    wordsList = body.split()
    print str(wordsList)
    sleep(2)
    for word in wordsList:
        if "up" in word:
      #      ser.write('0')
            sleep(1)
        elif "down" in word:
      #      ser.write('1')
            sleep(1)
        elif "left" in word:
      #      ser.write('2')
            sleep(1)
        elif "right" in word:
      #      ser.write('3')
            sleep(1)
        elif "paint" in word:
      #      ser.write('4')
            resp = twilio.twiml.Response()
            resp.message("Painted!")
            return str(resp)
            sleep(1)
        else:
            # Bad command
            resp = twilio.twiml.Response()
            resp.message("Oops! Try saying up, down, left, or right :)")
            return str(resp)
    # Good command
    resp = twilio.twiml.Response()
    if len(wordsList) > 1: resp.message("Moved around!")
    elif len(wordsList) == 1: resp.message("Moved %s!" % body.strip())
    return str(resp)


if __name__ == "__main__":
    app.run(debug=True)
