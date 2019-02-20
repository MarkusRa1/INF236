import matplotlib.pyplot as plt
# import os
import csv
import numpy as np
import pandas as pd
from sklearn.linear_model import LinearRegression

data = list()
# fn = os.path.join(os.path.dirname(__file__), '/../Report/report.csv')
with open('c:\\Users\\marku\\Documents\\INF236\\ProblemSet1\\Report\\report1D.csv', 'rb') as csvfile:
    csv_reader = csv.reader(csvfile, delimiter=',')
    nr = 0
    for row in csv_reader:
        if nr != 0:
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

ifigure = 0
dataTable = list()
for k in [10,11,12,13]:
    x = list()
    y = list()
    Xall = list()
    yall = list()
    for sblst in data:
        if sblst[1] == k:
            if int(sblst[2]) == 100000:
                x.append(int(sblst[0]))
                val = sblst[4]
                if val / 60 > 1:
                    if val / 60 / 60 > 1:
                        val = str(round(val / 60 / 60, 2)) + "h"
                    else:
                        val = str(round(val / 60, 2)) + "m"
                else:
                    val = str(round(val,2)) + "s"
                y.append(val)
    
    # predproc = list()
    # for proc in [1, 2, 4, 8, 16, 32, 64, 128]:
    #     if proc not in x:
    #         model = LinearRegression()
    #         xpred = list()
    #         ypred = list()
    #         for sblst in data:
    #             if proc == sblst[0]:
    #                 xpred.append([sblst[1]*sblst[1], int(sblst[2])])
    #                 ypred.append(sblst[3])
    #         model.fit(xpred, ypred)
    #         # print(xpred)
    #         predval = model.predict([[n*n, 10000]])[0]
    #         if predval / 60 > 1:
    #             if predval / 60 / 60 > 1:
    #                 predval = str(round(predval / 60 / 60, 2)) + "h"
    #             else:
    #                 predval = str(round(predval / 60, 2)) + "m"
    #         else:
    #             predval = str(round(predval,2)) + "s"
    #         y.append(predval)
    #         x.append(proc)
    #         predproc.append(proc)

    y = [yval for _,yval in sorted(zip(x,y))]
    x = sorted(x)

    dataTable.append(y)

    # for predx in predproc:
    #     ix = x.index(predx)
    #     x[ix] = str(predx) + '*'
    #     y[ix] = str(y[ix]) + '*'
    
    # plt.figure(ifigure)
    # ifigure += 1
    # index = np.arange(len(x))
    # # print(index)
    # # print(y)
    # rect = plt.bar(index, y, width=0.8)
    # autolabel(rect)
    # plt.xlabel('Number of processes')
    # plt.ylabel('Time in seconds')
    # plt.xticks(index, x)
    # plt.title('Time taken for n=' + str(n))
    # plt.show()

# fig, ax = plt.subplots()

# # hide axes
# fig.patch.set_visible(False)
# ax.axis('off')
# ax.axis('tight')

print(dataTable)
df = pd.DataFrame(np.array(dataTable))

# ax.table(cellText=df.values, colLabels=df.columns, loc='center')


# fig.tight_layout()

# plt.show()