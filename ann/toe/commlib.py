## /usr/bin/python3
## -*- coding: utf-8 -*-
# g_    global value
# c_    class value
# v_    parameter value
# m_    member value
# t_    temporary value

'''
  commlib.py
  by Pioevh@163.com 2010427
'''

import pdb
# pdb.set_trace()
import logging
logging.basicConfig(level=logging.DEBUG)
# logging.debug('This is debug message')
# logging.info('This is info message')
# logging.warning('This is warning message')
import time
import poplib
import sqlite3
import sys
import smtplib
import email.mime.multipart  
import email.mime.text

def spentTime(v_fun):
  '''
    decorator spentTime
  '''
  def t_decoratorfun(*v_para):
    t_starttime=time.time()
    t_rt=v_fun(*v_para)
    logging.debug('spentTime: '+'%5.6f'%((time.time()-t_starttime),)+' s.')
    return(t_rt)
  return(t_decoratorfun)

def detectCharSet(v_data):
  '''
    detect character set
    return member in ['utf-8','unicode','gb2312','gbk','gb18030','big5','us-ascii','unknow']
  '''
  t_types=['utf-8','unicode','gb2312','gbk','gb18030','big5','us-ascii','unknow']
  for t_codetype in t_types:
    try:
      v_data.decode(t_codetype)
      return(t_codetype)
    except:
      continue
  else:
    return('unknow')

def getMbFormat():
  '''
    get mb format
    return format
  '''
  return('{0}{3}{2}{1}{4}')

def getMbInner():
  '''
    get mb inner
    return mb
  '''
  return(getMbFormat().format('toe_inn','6.c','2','er@1','om'))

def getMbOuter():
  '''
    get mb outer
    return mb
  '''
  return(getMbFormat().format('toe_out','6.c','2','er@1','om'))
  
def getRxUrl():
  '''
    get receive url
    return url
  '''
  return(getMbFormat().format('po','6.c','2','p3.1','om'))

def getTxUrl():
  '''
    get transport url
    return url
  '''
  return(getMbFormat().format('sm','6.c','2','tp.1','om'))

def getPp():
  '''
    get pp
    return pp
  '''
  return(getMbFormat().format('76','to','29','08','e'))

def crypto(v_str):
  t_d={}
  for t_c in (65, 97):
    for t_i in range(26):
      t_d[chr(t_i+t_c)]=chr((t_i+13)%26+t_c)
  return((''.join([t_d.get(t_c,t_c) for t_c in v_str])))
  
def mbTx(v_mbFrom,v_mbTo,v_subject,v_content,v_pp):
  '''
    mb tx
  '''
  t_mb=email.mime.multipart.MIMEMultipart()
  t_mb['from']=v_mbFrom
  t_mb['to']=v_mbTo
  t_mb['subject']=v_subject
  t_mb.attach(email.mime.text.MIMEText(crypto(v_content)))
  t_smtp=smtplib.SMTP_SSL(getTxUrl())
  #t_smtp=smtplib.SMTP()
  #t_smtp.connect(getTxUrl())
  t_smtp.login(v_mbFrom,v_pp)
  t_smtp.sendmail(v_mbFrom,v_mbTo,str(t_mb))
  t_smtp.quit()

def mbRx(v_mb,v_pp,v_keyString):
  '''
    mb rx
  '''
  t_mb=poplib.POP3_SSL(getRxUrl())
  #t_mb=poplib.POP3(getRxUrl())
  t_mb.user(v_mb)
  t_mb.pass_(v_pp)
  t_mNumber=t_mb.stat()
  t_rt=[]
  t_contentFlag=False
  for i in range(1,t_mNumber[0]+1):
    #t_mlist=t_mb.top(i,1)
    t_mContent=[]
    for t_l in t_mb.retr(i)[1]:
      t_line=crypto(t_l.decode(detectCharSet(t_l)))
      if(t_contentFlag):
        t_mContent.append(t_line)
      if(t_line=='c/s %s 0'%(v_keyString,)):
        t_contentFlag=True
      if((t_mContent!=[])and(t_line=='c/s %s 1'%(v_keyString,))):
        t_rt.append(t_mContent[:-1])
        t_contentFlag=False
    t_mb.dele(i)
  t_mb.quit()
  return(t_rt)

def msg(v_msg,v_rollmode=True,v_newlinemode=True):
  '''
    print msg with roll and newline
  '''
  t_ctime=time.strftime("[%04Y-%02m-%02d_%2H:%2M:%2S] ",time.localtime())
  t_msg=('\n' if(v_newlinemode) else '')+t_ctime+v_msg
  if(v_rollmode==False):
    sys.stdout.write('\r'+(' '*80)+'\r'+t_msg)
  else:
    print(t_msg)
  sys.stdout.flush()
  
def sqliteOpt(v_db,v_opt):
  '''
    sqlite3 single operate
    return result
  '''
  t_rtArray=[]
  t_dbCon=sqlite3.connect(v_db)
  t_dbCur=t_dbCon.cursor()
  t_dbCur.execute(v_opt)
  t_rtArray=t_dbCur.fetchall()
  t_dbCon.commit()
  t_dbCur.close()
  t_dbCon.close()
  return(t_rtArray)

def sqliteOptBatch(v_db,v_opt):
  '''
    sqlite3 batch operate
  '''
  t_dbCon=sqlite3.connect(v_db)
  t_dbCur=t_dbCon.cursor()
  for t_el in v_opt:
    t_dbCur.execute(t_el)
  t_dbCon.commit()
  t_dbCur.close()
  t_dbCon.close()
  return(0)
  
@spentTime
def _test_():
  pass
  mbTx(getMbInner(),getMbOuter(),'pioevh\'s operation','\nc/s server 0\n%s\nc/s server 1\n'%('ioc > ios',),getPp())
  
if(__name__=='__main__'):
  _test_()
