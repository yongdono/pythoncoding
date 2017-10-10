import tushare as ts
import pandas as pd
import time

import sys
default_encoding = 'utf-8'
if sys.getdefaultencoding() != default_encoding:
    reload(sys)
    sys.setdefaultencoding(default_encoding)

#load convertible bond basic info
bondinfo=pd.read_csv('C:\\pythoncoding\\programming trade\\wmcloseplate\\convertible bond\\convertible bond basic data.csv')
bondcode=bondinfo['bondcode'] #get convertible bond code from csv
stockcode=bondinfo['stockcode']
conversion_price=bondinfo['conversion_price'] #get convertible bond conversion_price
bond2amount=100/conversion_price #get every bond(100yuan) transfer to amount of stock

'''
print bondcode
print type(bondcode)
print bond2amount
print type(bond2amount)
'''


while True:
    bondRealTimePrice=ts.get_realtime_quotes(['128009','113008','110030'])
    stockprice=ts.get_realtime_quotes(['002241','601727','600185'])
    stockpriceonly=stockprice["price"]

    for i in range(3):
		stockpriceonly[i]=float(stockpriceonly[i])

    #print bondRealTimePrice,stockprice
    stock2bondprice=bond2amount*stockpriceonly
    '''
	for i in range(3):
		print "bondinfo['%s'][%d]:stock2bondprice[%d]" %(bondinfo['name'][i],stock2bondprice[i])
   ''' 
    #bondpriceonly=bondRealTimePrice['price']
    time.sleep(5)
'''
    for j in range(3):
        bondpriceonly[j]=float(bondpriceonly[j])
    
	premium=abs(stock2bondprice-bondpriceonly)/stock2bondprice
    print premium
    print stock2bondprice
    print bondpriceonly
'''

