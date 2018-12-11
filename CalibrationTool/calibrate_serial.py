#!/usr/bin/python3

import sys
import os
import serial
import statistics
import csv

if len(sys.argv) < 2:
	print("No serial port specified")
	print("Windows Usage:\t\t\tpython calibrate_serial.py <port>")
	print("\t\t\t\t\twhere <port> is e.g. COM4")
	print("Linux Usage:\t\t\tcalibrate_serial.py <port>")
	print("\t\t\t\t\twhere <port> is e.g. /dev/tty1")
	exit()



port = sys.argv[1]; #write the com port on command line

outputfilename = 'calibration_data.csv'

alum = 'ALUMINIUM'
steel = 'STEEL'
white = 'WHITE'
black = 'BLACK'
samp = 'AVERAGE_SAMPLE_COUNT'
end = 'END'
classes = {alum: {'values':[],}, steel: {'values':[],}, white: {'values':[],}, black: {'values':[],},}
avgSampleCount = 0

try:
	try:
		ser = serial.Serial(port) # 9600, 8N1, no timeout
	except:
		print("Error: could not open serial port " + port + " for reading")
		print("Make sure this is the correct port for you device")
		exit()

	curr_item = ''
	count = 0

	while True:
		line = ser.readline().strip().decode('utf-8').strip('\x00')

		if line == end: # we have reached the end, break out and process
			ser.close()
			break

		if line in classes or line == samp:	# the line is a name of a class, update
			curr_item = line
			count = 0
			print(line)
		else:
			val = int(line)
			count += 1
			print('{}:\t{}'.format(count,val))
			if curr_item == samp:
				avgSampleCount = val
			else:
				classes[curr_item]['values'].append(val)


	#calculate means and standard deviations
	#throw error if any column only has one data point
	try:
		for (item_class,data) in classes.items():
			data['mean'] = statistics.mean(data['values'])
			data['stdev'] = statistics.stdev(data['values'])
	except statistics.StatisticsError as e:
		print('ERROR: ' + str(e))
		exit()

#	try:
	#Print statistical data to file and to screen
	with open(outputfilename, 'w', newline='') as outputfile:
		writer = csv.writer(outputfile, delimiter = ',')
		writer.writerow(['ItemClass', 'Mean', 'StDev'])

		for (item_class, data) in classes.items():
			writer.writerow([item_class, str(data['mean']), str(data['stdev'])])

		writer.writerow([])
		writer.writerow(['AvgSamp', avgSampleCount])
#	except:
#		print('ERROR: failed to open file ' + outputfilename + ' for writing')
	
	print('All done! The output data is located at: ' + os.path.join(os.getcwd(),outputfilename))
	exit()
except KeyboardInterrupt:
	ser.close()
	exit()