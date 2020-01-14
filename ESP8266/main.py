import time

while True:
	print("Temperature (in Celsius): " + str(s.get_temp_humi()[0]))
	print("Humidity: " + str(s.get_temp_humi()[1]))
	time.sleep(1)