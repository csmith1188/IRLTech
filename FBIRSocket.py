
import time
import socketio
import ir
import threading
from datetime import datetime
# Chris was here

# Create a Socket.IO client instance
sio = socketio.Client()

# Define connection event handler
@sio.event
def connect():
    print("Connected to the server!")
    sio.emit('getActiveClass')

@sio.event
def setClass(data):
   print("Got class", data)
   sio.emit('pollResp','True')

@sio.event
def disconnect():
    print("Disconnected from the server!")

@sio.event
def irSignal():
   print('Signal Received')


def ir_loop():
    try:
        while True:
            binary_signal = ir.getBinary()
            hex_signal = ir.convertHex(binary_signal)

            for name,code in ir.IRListener().buttons.items():
                if hex_signal == hex(code):
                    print(f"Button pressed: {name}")
                    if name == "1":
                        sio.emit('startPoll', ("3", False, "", [{"answer":"Up","weight":1,"color":"#00ff00"}, {"answer":"Wiggle","weight":1,"color":"#0000ff"}, {"answer":"Down","weight":1,"color":"#ff0000"}], {}, 1, [], [], [], [], False))
                    elif name == "2":
                        sio.emit('startPoll', ("2", False, "", [{"answer":"True","weight":1,"color":"#00ff00"}, {"answer":"False","weight":1,"color":"#ff0000"}], {}, 1, [], [], [], [], False))
                    elif name == "3":
                        sio.emit('startPoll', ("1", False, "", [{"answer":"Done/ready?","weight":1,"color":"#00ff00"}], {}, 1, [], [], [], [], False))
                    elif name == "4":
                        sio.emit('startPoll', ("4", False, "", [{"answer":"A","weight":1,"color":"#ff0000"}, {"answer":"B","weight":1,"color":"#00ff00"}, {"answer":"C","weight":1,"color":"#0000ff"}, {"answer":"D","weight":1,"color":"#A83295"}], {}, 1, [], [], [], [], False))
                    elif name == "play_pause":
                        sio.emit('clearPoll')
                    time.sleep(0.5)
                    break
    except KeyboardInterrupt:
        print("Exiting program...")
        GPIO.cleanup()
        sio.disconnect()

ir_thread = threading.Thread(target=ir_loop,daemon=True)
ir_thread.start()

# Get teacher API key
extra_headers = {
        "api": "13edef7790f96f0022ba1023040fc449da4d3cbb59ead5002524231599db72ea94f78d5e8ea6c54ffa52c38e5716fa444f9cd052332f7b0a554705fe52882692"
}

# Set to formbar URL
sio.connect('http://172.16.3.219:420', headers=extra_headers)

# Keep the client running to listen for events
sio.wait()
