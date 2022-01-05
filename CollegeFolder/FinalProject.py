
#  Board: Raspberry Pi-4 Model B
#  Operating System: Raspberry Pi OS
#  IDE: Visual Studio

#  Programmer(s) name: Miguel Cano, Emrul Alam, Imtiaz Ahmed, Kan Hui Zhao,
#  date when code is created/modified: 12/02/2021

#  The purpose of this code is to build a security system by using the following electrical components: RFID Tag and Reader, Membrane Matrix Keypad, LCD Display, Solenoid Door Lock.
   

#! /usr/bin/python

# import the necessary packages
from imutils.video import VideoStream
from imutils.video import FPS
import face_recognition
import time
import imutils
import pickle
import time
import cv2
import datetime
from flask import Flask, render_template, Response
import RPi.GPIO as GPIO
from RPLCD.i2c import CharLCD
import os

def process():

	app = Flask(__name__)
	@app.route('/')

	def index(): #flask server html codes
		#Video streaming home page
		return render_template('index.html')

	def write_data(): #for a write data to message.log (using send mail to admin in the other main code)
		w_file = open("message.log","w")
		w_file.write("Unknown")
		w_file.close()
		
	lcd = CharLCD('PCF8574', 0x27) #lcd screen driver and I2C channel

	def base_message(): #lcd screen base message
		lcd.clear()
		lcd.write_string("Face  Code  RFID")
		lcd.cursor_pos = (1, 0)
		lcd.write_string(" *     0      #")

	base_message()

	#Initialize 'currentname' to trigger only when a new person is identified.
	currentname = "unknown"
	#Determine faces from encodings.pickle file model created from train_model.py
	encodingsP = "encodings.pickle"
	#use this xml file
	cascade = "haarcascade_frontalface_default.xml"

	# load the known faces and embeddings along with OpenCV's Haar
	# cascade for face detection
	print("[INFO] loading encodings + face detector...")
	data = pickle.loads(open(encodingsP, "rb").read())
	detector = cv2.CascadeClassifier(cascade)

	# initialize the video stream and allow the camera sensor to warm up
	print("[INFO] starting video stream...")
	vs = VideoStream(src=0).start()
	#vs = VideoStream(usePiCamera=True).start()
	time.sleep(5)

	# start the FPS counter
	fps = FPS().start()


	def flasks():

		door=18 #which pin gives 5V to open door

		threshold=15 

		counter=0 #counter of unknown person's face
		counter_p=0 #counter of known person's face


		#keypad lines
		L1 = 5
		L2 = 6
		L3 = 13
		L4 = 19

		C1 = 12
		C2 = 16
		C3 = 20


		GPIO.setwarnings(False)
		GPIO.setmode(GPIO.BCM) #GPIO naming 


		#GPIO setup
		GPIO.setup(L1, GPIO.OUT)
		GPIO.setup(L2, GPIO.OUT)
		GPIO.setup(L3, GPIO.OUT)
		GPIO.setup(L4, GPIO.OUT)

		GPIO.setup(door,GPIO.OUT)
		GPIO.output(door,GPIO.LOW)

		GPIO.setup(C1, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
		GPIO.setup(C2, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
		GPIO.setup(C3, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

		star=False #keypad * icon
		zero=False #keypad 0 icon
		hashtag=False #keypad # icon

		password="1234" #keypad , unlocking code 
		activatealarm="4321" #keypad, activate alarm system

		global button
		button=""

		code=""
		digit=1 #keypad digit

		global push 
		push=False

		def control_door(): #controlling door
			GPIO.output(18,GPIO.HIGH)
			time.sleep(10)
			GPIO.output(18,GPIO.LOW)
			os.system('mosquitto_pub -d -u admin -P admin -t admin -m "0"') #deactivate alarm system

		def readLine(line, characters): #read keypad buttons
			global push
			global button
			
			if push==False:
				GPIO.output(line, GPIO.HIGH)

				#C mean column, L mean Line
				if(GPIO.input(C1) == 1):
					print(characters[0])
					button=characters[0]
					push=True

				if(GPIO.input(C2) == 1):
					print(characters[1])
					button=characters[1]	
					push=True

				if(GPIO.input(C3) == 1):
					print(characters[2])
					button=characters[2]
					push=True

				GPIO.output(line, GPIO.LOW)


		# loop over frames from the video file stream
		while True:

			GPIO.output(19, GPIO.HIGH)

			if(GPIO.input(C1) == 1): #if * pushed
				print("* pushed")
				star=True
				lcd.clear()
				lcd.write_string("Face Scanning!")

			if(GPIO.input(C2) == 1): #if 0 pushed
				print("0 pushed")
				zero=True

				lcd.clear()
				lcd.write_string("Please Enter")
				lcd.cursor_pos=(1,0)
				lcd.write_string("Password!")

				time.sleep(0.3)
				code=""


			if(GPIO.input(C3) == 1): #if # pushed
				print("# pushed")
				hashtag=True

				cv2.imwrite("image.png",frame) #take photo

				rfid_file = open("RFID.log","w") #write data to RFID.log (which communicates with other main code)
				rfid_file.write("True")
				rfid_file.close()

			GPIO.output(19, GPIO.LOW)

			# grab the frame from the threaded video stream and resize it
			# to 500px (to speedup processing)

			frame = vs.read()
			frame1=frame.copy() #for flask server
			frame = imutils.resize(frame, width=500)

			if zero: # if keypad is true

				#read buttons
				readLine(L1, ["1","2","3"])
				readLine(L2, ["4","5","6"])
				readLine(L3, ["7","8","9"])
				readLine(L4, ["*","0","#"])
				time.sleep(0.15)

				if digit<=4 and push: #if code digit <4
					code=code+str(button)
					print("code: " +code)

					lcd.clear()
					lcd.write_string("Code: "+code)

					digit=digit+1
					push=False

				if digit>4: #if code digit =
					print("code: " +code)

					if (code !=password) or (code !=activatealarm):
						print("False Code")

						cv2.imwrite("image.png",frame) #take photo
						write_data() 

						lcd.clear()
						lcd.write_string("False Code")
						lcd.cursor_pos=(1,0)
						lcd.write_string("Try Again!")
						time.sleep(2)
						base_message()

					if code==password:
						print("True Code")
						lcd.clear()
						lcd.write_string("Right Code")
						lcd.cursor_pos=(1,0)
						lcd.write_string("Door Opened!")
						control_door()
						base_message()

					if code==activatealarm:
						print("True Code for Alarm System")
						lcd.clear()
						lcd.write_string("Right Code")
						lcd.cursor_pos=(1,0)
						lcd.write_string("Alarm Activated")
						os.system('mosquitto_pub -d -u admin -P admin -t admin -m "1"') #activate alarm system
						time.sleep(2)
						base_message()
	

					zero=False
					push=False
					digit=1
					code=""
	
			if star:
			
				# convert the input frame from (1) BGR to grayscale (for face
				# detection) and (2) from BGR to RGB (for face recognition)
				gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
				rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

				# detect faces in the grayscale frame
				rects = detector.detectMultiScale(gray, scaleFactor=1.1, 
					minNeighbors=5, minSize=(30, 30),
					flags=cv2.CASCADE_SCALE_IMAGE)

				# OpenCV returns bounding box coordinates in (x, y, w, h) order
				# but we need them in (top, right, bottom, left) order, so we
				# need to do a bit of reordering
				boxes = [(y, x + w, y + h, x) for (x, y, w, h) in rects]

				# compute the facial embeddings for each face bounding box
				encodings = face_recognition.face_encodings(rgb, boxes)
				names = []

				# loop over the facial embeddings
				for encoding in encodings:
					# attempt to match each face in the input image to our known
					# encodings
					matches = face_recognition.compare_faces(data["encodings"],
						encoding)
					name = "Unknown" #if face is not recognized, then print Unknown

					# check to see if we have found a match
					if True in matches:
						counter_p=counter_p+1
						print("p"+str(counter_p))
						lcd.cursor_pos=(1,0)
						lcd.write_string("%"+str(counter_p*20))
						counter=0

						# find the indexes of all matched faces then initialize a
						# dictionary to count the total number of times each face
						# was matched
						matchedIdxs = [i for (i, b) in enumerate(matches) if b]
						counts = {}

						# loop over the matched indexes and maintain a count for
						# each recognized face face
						for i in matchedIdxs:
							name = data["names"][i]
							counts[name] = counts.get(name, 0) + 1

						# determine the recognized face with the largest number
						# of votes (note: in the event of an unlikely tie Python
						# will select first entry in the dictionary)
						name = max(counts, key=counts.get)

						if counter_p>=5: #if last 5 frame is known person's face
							print("Door opened - "+ name)

							lcd.clear()
							lcd.write_string("Hello " +name)
							lcd.cursor_pos=(1,0)
							lcd.write_string("Door Opened!")
							control_door()
							base_message()
							counter_p=0
							star=False
						
						
						#If someone in your dataset is identified, print their name on the screen
					
					if name=="Unknown":
						counter=counter+1
						print("c"+str(counter))
						lcd.cursor_pos=(1,0)
						lcd.write_string("%"+str(counter*20))
						counter_p=0

						if counter>=5: #if last 5 frame is unknown person's face
							cv2.imwrite("image.png",frame)
							write_data()

							lcd.clear()
							lcd.write_string("Unknown Face")
							lcd.cursor_pos=(1,0)
							lcd.write_string("Please Wait!")

							print("Unknown face detected")
							star=False

					
					# update the list of names
					names.append(name)

				# # loop over the recognized faces
				# for ((top, right, bottom, left), name) in zip(boxes, names):
				# 	# draw the predicted face name on the image - color is in BGR
				# 	cv2.rectangle(frame, (left, top), (right, bottom),
				# 		(0, 255, 225), 2)
				# 	y = top - 15 if top - 15 > 15 else top + 15
				# 	cv2.putText(frame, name, (left, y), cv2.FONT_HERSHEY_SIMPLEX,
				# 		.8, (0, 255, 255), 2)

				# display the image to our screen
				cv2.imshow("Facial Recognition", frame)

				if counter>5:
					counter=0

			ret, buffer = cv2.imencode('.jpg', frame1) #compress and store image to memory buffer
			frame1 = buffer.tobytes()
			yield (b'--frame\r\n'b'Content-Type: image/jpeg\r\n\r\n' + frame1 + b'\r\n') #concat frame one by one and return frame (for flask server)

			key = cv2.waitKey(1) & 0xFF
			

			# quit when 'q' key is pressed
			if key == ord("q"):
				break

			# update the FPS counter
			fps.update()

		# stop the timer and display FPS information
		fps.stop()
		print("[INFO] elasped time: {:.2f}".format(fps.elapsed()))
		print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))

		# do a bit of cleanup
		cv2.destroyAllWindows()
		vs.stop()

	@app.route('/video_feed')
	def video_feed():
	#Video streaming route
		return Response(flasks(),mimetype='multipart/x-mixed-replace; boundary=frame')
	if __name__ == "__main__":
		app.run(host="192.168.1.218",port='5000', debug=False,threaded = True)

process()
