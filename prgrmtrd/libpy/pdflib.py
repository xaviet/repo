#!/usr/bin/env python3
# coding=utf-8

'''
pdflib.py
'''

import sys
import re
import os
import time
import io
from pdfminer.pdfinterp import PDFResourceManager, PDFPageInterpreter
from pdfminer.converter import TextConverter
from pdfminer.layout import LAParams
from pdfminer.pdfpage import PDFPage

def convertpdftotxt(v_path):
  t_rsrcmgr=PDFResourceManager()
  t_retstr=io.StringIO()
  t_laparams=LAParams()
  t_device=TextConverter(t_rsrcmgr,t_retstr,laparams=t_laparams)
  t_fp=open(v_path,'rb')
  t_interpreter=PDFPageInterpreter(t_rsrcmgr,t_device)
  t_pagenos=set()
  for t_page in PDFPage.get_pages(t_fp,t_pagenos):
    t_interpreter.process_page(t_page)
  t_fp.close()
  t_device.close()
  t_str=t_retstr.getvalue()
  t_retstr.close()
  return t_str


if(__name__=='__main__'):
  print(convertpdftotxt('1.pdf'))
