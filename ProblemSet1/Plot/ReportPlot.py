import matplotlib.pyplot as plt
# import os
import csv
import numpy as np

data = list()
# fn = os.path.join(os.path.dirname(__file__), '/../Report/report.csv')
with open('c:\\Users\\marku\\Documents\\INF236\\ProblemSet1\\Report\\report.csv', 'rb') as csvfile:
    csv_reader = csv.reader(csvfile, delimiter=',')
    nr = 0
    for row in csv_reader:
        if nr != 0 and int(row[2]) == 10000:
            row = np.array(map(float, row))
            row[0] = int(row[0])
            row[1] = int(row[1])
            row[2] = int(row[2])
            data.append(map(float,row))
        nr += 1

def autolabel(rects):
    """
    Attach a text label above each bar displaying its height
    """
    for rect in rects:
        height = rect.get_height()
        plt.text(rect.get_x() + rect.get_width()/2., 0,
                '%ds' % int(height),
                ha='center', va='bottom')

for n in [1024, 2048, 4096]:
    x = list()
    y = list()
    for sblst in data:
        if sblst[1] == n:
            print(sblst)
            x.append(int(sblst[0]))
            y.append(int(sblst[3]))
    plt.figure(n)
    index = np.arange(len(x))
    rect = plt.bar(index, y, width=0.8)
    autolabel(rect)
    plt.xlabel('Size of n')
    plt.ylabel('Time in seconds')
    plt.xticks(index, x, rotation=30)
    plt.title('Time taken for p=' + str(n) + ' processes')
    plt.show()