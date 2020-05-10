"""
Python module for parsing and plotting values found in the
logs created by the Weather_Project.

Installation:
    (install python 3)
    python -m venv weather-env
    source weather-env/bin/activate
    pip install matplotlib

Usage:
    source weather-env/bin/activate
    python plotData.py <path/to/logfile.txt>
"""

from datetime import datetime

import matplotlib.pylab as plt

def parseDataLines(key, lines):
    "Take lines from file and return just numerical data we want"

    # get only the lines we care about, getting rid of the
    # trailing '\n'
    dataLines = [l[:-1] for l in lines if key in l]

    # Ex: 'moisture1: 34.0' -> 34.0
    # get the actual value:
    data = [int(l.split(':')[1].strip()) for l in dataLines]
    
    return data

def plotDataLines(key, lines, dts, fltr):
    "Plots data that begins with given key in given lines from file."

    data = parseDataLines(key, lines)

    # post parsing filtering?
    if fltr is not None:
        data = [d for d in data if fltr(d)]

    print("Plotting %s with %d points" % (key, len(data)))

    # now plot it
    plotDataByIndex(data, key)

def plotDataByIndex(data, key):    
    f = plt.figure()
    ax = f.gca()
    ax.plot(data)
    title = key.replace(" ", "_")
    plt.title(key)
    plt.savefig(key)

def dtStr2dt24(dtStr):
    "4:04:53 | Thursday - 4/16/20 -> datetime obj"

    # AM:PM format
    timeStr = dtStr.split("|")[0].strip()

    # amPm = timeStr[-2:]

    dateStr = dtStr.split("-")[1].strip()

    frmt = "%H:%M:%S %m/%d/%y"

    dtStr2 = "%s %s" % (timeStr, dateStr)
    return datetime.strptime(dtStr2, frmt)

def dtStr2dtAMPM(dtStr):
    "4:04:53PM | Thursday - 4/16/20 -> datetime obj"
    # AM:PM format
    timeStr = dtStr.split("|")[0].strip()

    amPm = timeStr[-2:]

    dateStr = dtStr.split("-")[1].strip()

    frmt = "%I:%M:%S %p %m/%d/%y"

    dtStr2 = "%s %s %s" % (timeStr[:-2], amPm, dateStr)
    return datetime.strptime(dtStr2, frmt)

    # print (dtStr2)
    # try:
    #     return datetime.strptime(dtStr2, frmt)
    # except:
    #     return None

def dtStr2dt(dtStr):
    "4:04:53[PM] | Thursday - 4/16/20 -> datetime obj"

    if "AM" in dtStr or "PM" in dtStr:
        return dtStr2dtAMPM(dtStr)
    else:
        return dtStr2dt24(dtStr)

def getDatetimes(ls):
    "Find the timestamps in the text log and return their datetime objs"

    bads = 0
    dts = []
    for i, l in enumerate(ls):
        if l == "------------\n":
            if i+1 < len(ls):
                dtStr = ls[i+1]
                try:
                    dt = dtStr2dt(dtStr)
                    dts.append(dt)
                except:
                    print("bad date:", dtStr)    
                    bads += 1    

    print("parsed %d datetimes, with %d errors" % (len(dts), bads))                
    return dts 
        
def windFilter(value):
    return value < 98

def plotDataWithoutTimestamps(filename):
    "Read given file and plot each type of data by index."

    with open(filename, 'r') as f:
        ls = f.readlines()

    # data looks like this:    
    #------------100
    #.7
    #isTooDry: 1
    #Moisture 1: 118
    #Too Dry Value 1: 146
    #Moisture 2: 121
    #Too Dry Value 2: 155


    dts = getDatetimes(ls)

    keys = [
       ( "Moisture 1", None),
        ("Moisture 2", None),
       ( "Too Dry Value 1", None),
       ( "Too Dry Value 2", None),
       ( "isTooDry", None),
       ( "Wind Reading", None)
    ]
    for key, fltr in keys:
        plotDataLines(key, ls, dts, fltr)

def plotData(filename):
    "Read given file and plot each type of data"

    with open(filename, 'r') as f:
        ls = f.readlines()

    print("file %s has %d number of lines" % (filename, len(ls)))
    # data comes in chunks that look like:
    """    
    ------------
    23:40:44 | Monday - 4/20/20
    isTooDry: 0
    Moisture 1: 0
    Too Dry Value 1: 0
    Moisture 2: 0
    Too Dry Value 2: 0
    Wind Reading: 98    
    """

    # go through each line of data and once the separator is 
    # encountered, parse the next few lines as a group, 
    # so that the data can be timestamped
    separator = "------------\n"

    # this will help us parse each chunk
    # (name, offset from separator, filter function)
    keys = [
       ( "isTooDry", 2, None),
       ( "Moisture 1", 3, None),
       ( "Too Dry Value 1", 4, None),
        ("Moisture 2", 5, None),
       ( "Too Dry Value 2", 6, None),
       ( "Wind Reading", 7, windFilter)
    ]
    chunkLen = 8
    invalidChunks = 0

    # here's how we'll store the parsed data
    data = {}
    for key, offset, fltr in keys:
        data[key] = []

    for i, l in enumerate(ls):
        if l == separator:
            # ignore the last chunk if it is not complete
            if i+9 > len(ls):
                continue

            # don't bother trying to parse chunks that
            # aren't the right format    
            if ls[i+chunkLen] != separator:
                # print("invalid chunk, skipping")
                # print(ls[i+chunkLen])
                invalidChunks += 1
                continue

            # what's the timestamp for this chunk?    
            dt = dtStr2dt(ls[i+1])
            # go through each line of the chunk
            for key, offset, fltr in keys:
                v = getValue(ls[i+offset], key, fltr)
                if v is not None:
                    # we've got data!, store both the value and when we got it
                    data[key].append((dt, v))

    print("Num invalid chunks: ", invalidChunks)
    for key, value in data.items():
        print(key, len(value))
        # separate out timestamps
        keyData = [d for dt, d in value]
        dts = [dt for dt, d in value]
        plotDataByDatetime(keyData, dts, key)
    
def getValue(l, key, fltr):
    "'Too Dry Value 1: 0' -> 0"
    keyValue = l.split(':')[0].strip()
    # assert keyValue == key
    if keyValue != key:
        print ("bad", keyValue, key)
        return None
    value = l[:-1].split(':')[1].strip() 
    value = int(value) 
    if fltr is not None:
        if fltr(value):
            return value
        else:
            return None    
    else:                                      
        return value

def plotDataByDatetime(data, dts, key):    
    f = plt.figure()
    ax = f.gca()
    ax.plot_date(dts, data, xdate=True, markersize=8)
    ax.xaxis.set_tick_params(rotation=30, labelsize=10)
    title = key.replace(" ", "_")
    plt.title(key)
    plt.savefig(key)

def main():
    import sys
    filename = sys.argv[1]
    plotData(filename)

if __name__ == '__main__':
    main()
