#from pyrebaseconfig import firebaseconfig
from examplepy import examplepy

config = {
  "apiKey": "AIzaSyDVu-US7miNyUeEXypYioP-tIgXLq3K9Zg",
  "authDomain": "buddysitter-a529c-default-rtdb.europe-west1.firebasedatabase.app/",
  "databaseURL": "https://buddysitter-a529c-default-rtdb.europe-west1.firebasedatabase.app/",
  "storageBucket": "buddysitter-a529c-default-rtdb.europe-west1.appspot.com"
}

firebase = examplepy.initialize_app(config)

print("Pyrebase Initialized")

    # Get a reference to the auth service
auth = firebase.auth()

        # Log the user in
user = auth.sign_in_with_email_and_password("hugomff@gmail.com", "123456")
print(user)
