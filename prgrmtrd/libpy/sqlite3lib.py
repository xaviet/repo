#!/usr/bin/env python3
# coding=utf-8

'''
sqlite3lib.py
'''

import os
import sqlite3

dbpath='..{0}db{1}'.format(os.sep,os.sep)
dailydb='{0}daily{1}'.format(dbpath,os.sep)
tradedatadb='{0}tradedata{1}'.format(dbpath,os.sep)
bigvolumedb='{0}{1}.sdb'.format(tradedatadb,'bigvolume')

def optrecod(v_db,v_opt):
  t_rtarray=[]
  t_dbcon=sqlite3.connect(v_db)
  t_dbcur=t_dbcon.cursor()
  t_dbcur.execute(v_opt)
  t_rtarray=t_dbcur.fetchall()
  t_dbcon.commit()
  t_dbcur.close()
  t_dbcon.close()
  return(t_rtarray)

def optrecodbatch(v_dbname,v_dboptbatch):
  t_dbcon=sqlite3.connect(dbpath+v_dbname)
  t_dbcur=t_dbcon.cursor()
  for t_el in v_dboptbatch:
    t_dbcur.execute(t_el)
  t_dbcon.commit()
  t_dbcur.close()
  t_dbcon.close()
  return(0)
  
if(__name__=='__main__'):
  print(tradedatadb)
