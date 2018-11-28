#!/usr/bin/python3

import csv
import requests
import statistics

tempfilename = 'temp.csv'
outputfilename = 'calibration_data.csv'
sheet_url = 'https://docs.google.com/spreadsheets/d/1MDKEVvNEbF4jrLezksOLjtHy_6yxtQnqf8HEoKLRfe8/export?format=csv&id=1MDKEVvNEbF4jrLezksOLjtHy_6yxtQnqf8HEoKLRfe8'

response = requests.get(sheet_url)
if response.status_code != 200:
	print("ERROR: could not pull data from Google Drive -- Error" + str(response.status_code))

csvcontent = response.content

try:
	with open(tempfilename, 'w') as tempfile:
		tempfile.write(csvcontent.decode('utf-8'))
except:
	print("ERROR: failed to open" + tempfilname)

try:
	with open(tempfilename, newline='') as csvfile:
		reader = csv.reader(csvfile, delimiter=',')

		cols = {'aluminium': {'values':[],}, 'steel': {'values':[],}, 'white': {'values':[],}, 'black': {'values':[],},}

		rows = iter(reader)
		next(rows)

		for row in rows:
			cols['aluminium']['values'].append(int(row[0],2))
			cols['steel']['values'].append(int(row[1],2))
			cols['white']['values'].append(int(row[2],2))
			cols['black']['values'].append(int(row[3],2))

		#calculate means and standard deviations
		#throw error if any column only has one data point
		try:
			for (col,data) in cols.items():
				data['mean'] = statistics.mean(data['values'])
				data['stdev'] = statistics.stdev(data['values'])
		except statistics.StatisticsError as e:
			print('ERROR: ' + str(e))
			exit()

		try:
			#Print statistical data to file and to screen
			with open(outputfilename, 'w', newline='') as outputfile:
				writer = csv.writer(outputfile, delimiter = ',')
				writer.writerow(['ItemClass', 'Mean', 'StDev'])

				for (col, data) in cols.items():
					writer.writerow([col, str(data['mean']), str(data['stdev'])])
		except:
			print('ERROR: failed to open file ' + outputfilename + ' for writing')
except:
	print('ERROR: failed to open tempfile for reading')
	exit()