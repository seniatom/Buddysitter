#!/usr/bin/env python3
from pyrebase import pyrebase

global user
global firebase
global localId

def FirebaseInit():
    config = {
    "apiKey": "AIzaSyDVu-US7miNyUeEXypYioP-tIgXLq3K9Zg",
    "authDomain": "buddysitter-a529c-default-rtdb.europe-west1.firebasedatabase.app/",
    "databaseURL": "https://buddysitter-a529c-default-rtdb.europe-west1.firebasedatabase.app/",
    "storageBucket": "buddysitter-a529c.appspot.com",
    "serviceAccount": "/etc/buddysitter.json"
    }

    global firebase
    firebase = pyrebase.initialize_app(config)
    print("Pyrebase Initialized")
    
    return 1

def LoginUser(userId):
    global localId
    localId = userId
    return 1

def GetStreamingFlag():
    # Get a reference to the database service
    db = firebase.database()

    # Pass the user's idToken to the get method
    flag = db.child(localId).child("Flags").child("Streaming_Flag").get()

    return(flag.val())

def GetSpeakerFlag():
    # Get a reference to the database service
    db = firebase.database()

    # Pass the user's idToken to the get method
    flag = db.child(localId).child("Flags").child("Speaker_Flag").get()
    
    return(flag.val())
    
def GetChangeWifiFlag():
    # Get a reference to the database service
    db = firebase.database()

    # Pass the user's idToken to the get method
    flag = db.child(localId).child("Flags").child("Change_Wifi_Flag").get()
    
    return(flag.val())
    
def GetSchedulingTimes(cur_year, cur_month, cur_day, cur_hour, cur_minute):
    # Get a reference to the database service
    db = firebase.database()
    
    # Pass the user's idToken to the get method
    header = db.child(localId).child("Scheduled_Times").get()
    for x in header.each():
        year = db.child(localId).child("Scheduled_Times").child(x.key()).child("Year").get().val()
        month = db.child(localId).child("Scheduled_Times").child(x.key()).child("Month").get().val()
        day = db.child(localId).child("Scheduled_Times").child(x.key()).child("Day").get().val()
        hour = db.child(localId).child("Scheduled_Times").child(x.key()).child("Hour").get().val()
        minute = db.child(localId).child("Scheduled_Times").child(x.key()).child("Minute").get().val() 
        if year == cur_year and month == cur_month and day == cur_day and hour == cur_hour and minute == cur_minute:
            weight = db.child(localId).child("Scheduled_Times").child(x.key()).child("Weight").get().val()
            db.child(localId).child("Scheduled_Times").child(x.key()).remove()
            return weight
            
    return 0

def DownloadAudio(index):
    storage = firebase.storage()
    voice_command = "audio" + index + ".m4a"
    cloud_path = "/" + localId + "/voice/" + voice_command 
    storage.child(cloud_path).download(voice_command)
    
    return 0

def ResetAudioValue():
    # Get a reference to the database service
    db = firebase.database()
    
    db.child(localId).child("Flags").update({"Speaker_Flag": 0})

    return 0
