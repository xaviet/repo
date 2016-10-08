#!/usr/bin/env python3
# coding=utf-8

'''
httplib.py
'''

import re
import gzip
import socket
import urllib.request
import commonlib
from html.parser import HTMLParser
import socketlib

class htmltableformat(HTMLParser):

  def __init__(self):
    HTMLParser.__init__(self)
    self.m_contents=[]
    self.m_flag=False
    self.m_tablevalue=''
    self.m_tablerowitems=0
    self.m_tableattr=''
    self.m_el=[]
    self.m_count=0
    pass

  def handle_starttag(self,tag,attrs):  
    if(tag=='table'):
      for attr,value in attrs:
        if((attr==self.m_tableattr)and(value==self.m_tablevalue)):
          self.m_flag=True  

  def handle_endtag(self,tag):  
    if(tag=='table'):  
      self.m_flag=False  

  def handle_data(self,data):  
    t_data=''
    if(self.m_flag):
      t_data=data.strip();
      if(len(t_data)>0):
        self.m_count+=1
        self.m_el.append(t_data)
        if(self.m_count==self.m_tablerowitems):
          self.m_count=0
          self.m_contents.append(self.m_el)
          self.m_el=[]
          
def detectcharset(v_data):
  #add item:['utf-8','gb2312','gbk','gb18030','iso-8859-1','unknow']
  t_types=['utf-8','unicode','gb2312','gbk','gb18030','big5','unknow']
  for t_codetype in t_types:
    try:
      v_data.decode(t_codetype)
      break
    except:
      pass
  return(t_codetype)
  
def gethtml(v_htmladdress,v_htmlcode='utf-8',v_user=None,v_pwd=None):
  t_urlheaders={'User-Agent':'Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.114 Safari/537.36','Accept-Charset':'utf-8'}
  t_charset=v_htmlcode
  socket.setdefaulttimeout(8)
  t_opener=urllib.request.build_opener()
  t_opener.addheaders=[t_urlheaders]
  if(v_user!=None):
    t_pwdMsg=urllib.request.HTTPPasswordMgrWithDefaultRealm()  
    t_pwdMsg.add_password(None,v_htmladdress,v_user,v_pwd)  
    t_auth=urllib.request.HTTPBasicAuthHandler(t_pwdMsg)  
    t_opener.add_handler(t_auth)    
  try:
    t_data=t_opener.open(v_htmladdress).read()
    t_opener.close()
    t_charset=detectcharset(t_data[0:8])
    if t_charset=='unknow':
      t_flows=io.BytesIO(t_data)
      t_data=gzip.GzipFile(fileobj=t_flows,mode="rb").read()
      t_charset=v_htmlcode
    t_decodedata=t_data.decode(t_charset,'ignore')
  except:
    t_decodedata='fail[{0}]'.format(v_htmladdress)
  return(t_decodedata)

def accessurl(v_url,v_code='utf-8'):
  commonlib.msg(v_url[16:64]+' accessing',False)
  t_repeat=0
  t_str='fail'
  while(t_str[0]=='f'):
    t_repeat+=1
    t_str=gethtml(v_url,v_code)
    if((t_repeat%64)==0):
      commonlib.msg(v_url,True,True)
      commonlib.msg('fail(access times big than 64)',True,True)
      break
  return(t_str)
  
def bigvolumedataformat(v_str,v_specvalue,v_lastend=100):
  t_array=[]
  t_htmltableformat=htmltableformat() 
  t_htmltableformat.m_tablevalue='table_bg001 border_box limit_sale'
  t_htmltableformat.m_tableattr='class'
  t_htmltableformat.m_tablerowitems=5
  t_htmltableformat.feed(v_str)
  t_htmltableformat.close()
  t_allbigvolume=t_htmltableformat.m_contents
  for t_el in t_allbigvolume:
    if(t_el[4]=='买盘'):
      t_rtv=int(re.sub(',','',t_el[2]))
      t_extime=int(re.sub(':','',t_el[0]))
      t_bvprice=float(t_el[1])
      if((t_rtv>=v_specvalue)and(v_lastend>t_bvprice)and((t_extime>94000)and(t_extime<145700))):
        t_array.append(t_bvprice)
        t_array.append(t_extime)
        break
  return(t_array)
    
def getbigvolumedata(v_code,v_specvalue,v_level,v_lastend=100):
  t_array=[]
  t_code=commonlib.codei2o(str(v_code))
  t_url='http://quotes.money.163.com/trade/ddtj_{0}.html?amount={1}'.format(t_code[2:len(t_code)],str(v_level))
  t_str=accessurl(t_url,'gbk') 
  t_array=bigvolumedataformat(t_str,v_specvalue,v_lastend)
  if(len(t_array)==2):
    t_array.append(v_code)
    t_array.append(v_lastend)
  else:
    t_array=[]
  return(t_array) #price;time;code;lastend
  
def sinadataformat(v_str):
  t_str=v_str
  t_str=re.sub('=',',',t_str)
  t_str=re.sub('"','',t_str)
  t_array0=t_str.split('\n')
  del t_array0[len(t_array0)-1]
  t_array1=[]
  for t_el in t_array0:
    t_el=t_el[11:len(t_el)-2]
    t_array2=t_el.split(',')
    if(len(t_array2)>31):
      t_array2[0]=commonlib.codeo2i(t_array2[0])  #code
      t_array2[31]=re.sub('-','',t_array2[31])  #date
      t_array1.append(t_array2)
  return(t_array1)
  
def getsinadata(v_array):
  t_str='http://hq.sinajs.cn/list='+commonlib.array2string(v_array,',')
  t_str=accessurl(t_str)
  return(sinadataformat(t_str))

if(__name__=='__main__'):
  print(gethtml('http://linux.linuxidc.com/',v_user='www.linuxidc.com',v_pwd='www.linuxidc.com'))
  print(gethtml('http://www.linuxidc.com/'))
