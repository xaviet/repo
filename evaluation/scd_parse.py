# /usr/bin/python3
# -*- coding: utf-8 -*-

import xml_parse

def dens(xmltag):
  nsend=xmltag.find('}')
  return(xmltag if(nsend==-1) else xmltag[nsend+1:])

def getieddesc(xmlnode):
  ieddescdict={}
  for el0 in xmlnode.iter():
    if(dens(el0.tag)=='Substation'):
      for el1 in el0.iter():
        if(dens(el1.tag).find('IED')!=-1):
          if(('name' in el1.attrib)and('desc' in el1.attrib)):
            attrib=el1.attrib
            if(attrib['name'] in ieddescdict):
              print('Warnning! iedname: '+attrib['name']+' duplicate.')    
              break      
            ieddescdict[attrib['name']]=attrib['desc']
  return(ieddescdict)

def gettextmatchattrib(node,attclass,attvalue,defaultvalue):
  rt=defaultvalue
  attrib=node.attrib
  if(attclass in attrib):
    if(attvalue==attrib[attclass]):
      rt=node.text
  return(rt)

def datasetinfo(xmlnode,datasetname):
  dsinfo={}
  for el0 in xmlnode.iter():
    if('DataSet'==dens(el0.tag)):
      attrib=el0.attrib
      if(attrib['name']==datasetname):
        desc=attrib['desc'] if('desc' in attrib) else ''
        channels=0
        for el1 in el0.iter():
          if(dens(el1.tag)=='FCDA'):
            channels+=1
        if(dsinfo!={}):
          print('Warnning! dataset: %s duplicate.'%(datasetname))    
        if(channels==0):
          print('Warnning! dataset: %s, channel is empty.'%(datasetname))    
        else:
          dsinfo={'channels':str(channels),'desc':desc}     
  return(dsinfo)

def iedinputs(xmlnode):
  iedinputs=[]
  for el0 in xmlnode.iter():
    if(dens(el0.tag)=='Inputs'):
      for el1 in el0.iter():
        if(dens(el1.tag)=='ExtRef'):
          extref=el1.attrib
          if('iedName' in extref):
            if(extref['iedName'] not in iedinputs):
              iedinputs.append(extref['iedName'])
  if(iedinputs==[]):
    print('Warnning! LDevice: %s inputs is empty.'%(dens(xmlnode.tag)))    
  return(iedinputs)

def getgsedataset(xmlnode,iedName,apName,ldInst,cbName):
  dataset={}
  for el0 in xmlnode.iter():
    if(dens(el0.tag)=='IED'):
      attrib=el0.attrib
      if(attrib['name']==iedName):
        for el1 in el0.iter():
          if(dens(el1.tag)=='AccessPoint'):
            attrib=el1.attrib;
            if(attrib['name']==apName):
              for el2 in el1.iter():
                if(dens(el2.tag)=='LDevice'):
                  attrib=el2.attrib
                  if(attrib['inst']==ldInst):
                    for el3 in el2.iter():
                      if(dens(el3.tag)=='GSEControl'):
                        attrib=el3.attrib
                        if(attrib['name']==cbName):
                          if(dataset!={}):
                            print('Warnning! dataset: [%s, %s, %s, %s] duplicate.'%(iedName,apName,ldInst,cbName))    
                          else:
                            dataset={'datSet':attrib['datSet'],'confRev':attrib['confRev'],'appID':attrib['appID'],'datasetinfo':datasetinfo(el2,attrib['datSet']),'inputs':iedinputs(el2)}
  return(dataset)

def getsmvdataset(xmlnode,iedName,apName,ldInst,cbName):
  dataset={}
  for el0 in xmlnode.iter():
    if(dens(el0.tag)=='IED'):
      attrib=el0.attrib
      if(attrib['name']==iedName):
        for el1 in el0.iter():
          if(dens(el1.tag)=='AccessPoint'):
            attrib=el1.attrib;
            if(attrib['name']==apName):
              for el2 in el1.iter():
                if(dens(el2.tag)=='LDevice'):
                  attrib=el2.attrib
                  if(attrib['inst']==ldInst):
                    for el3 in el2.iter():
                      if(dens(el3.tag)=='SampledValueControl'):
                        attrib=el3.attrib
                        if(attrib['name']==cbName):
                          if(dataset!={}):
                            print('Warnning! dataset: [%s, %s, %s, %s] duplicate.'%(iedName,apName,ldInst,cbName))    
                          else: 
                            dataset={'datSet':attrib['datSet'],'confRev':attrib['confRev'],'smvID':attrib['smvID'],'datasetinfo':datasetinfo(el2,attrib['datSet']),'inputs':iedinputs(el2)} 
                                
  return(dataset)

def getappid(xmlnode,type='GSE'):
  appiddict={}
  for el0 in xmlnode.iter():
    if(dens(el0.tag)=='Communication'):
      for el1 in el0.iter():
        if(dens(el1.tag).find('ConnectedAP')!=-1):
          if(('iedName' in el1.attrib)and('apName' in el1.attrib)):
            iedName,apName,cbName,ldInst='','','',''
            mac,appid,vlanid,vlanpriority='','','',''
            attrib=el1.attrib
            iedName=attrib['iedName']
            apName=attrib['apName']
            for el2 in el1.iter():
              if(dens(el2.tag).find(type)!=-1):
                if(('cbName' in el2.attrib)and('ldInst' in el2.attrib)):
                  attrib=el2.attrib
                  cbName=attrib['cbName']
                  ldInst=attrib['ldInst']
                  for el3 in el2.iter():
                    appid=gettextmatchattrib(el3,'type','APPID',appid)
                    if(appid!=''):
                      appid='%x'%int(appid,16)
                      if(appid in appiddict):
                        print('Warnning! appid: 0x'+appid+' duplicate.')    
                        break                  
                    mac=gettextmatchattrib(el3,'type','MAC-Address',mac)
                    vlanid=gettextmatchattrib(el3,'type','VLAN-ID',vlanid)
                    vlanpriority=gettextmatchattrib(el3,'type','VLAN-PRIORITY',vlanpriority)
            if((appid!='') and (appid not in appiddict)):
              dateset={}
              dateset=getgsedataset(xmlnode,iedName,apName,ldInst,cbName) if(type=='GSE') else getsmvdataset(xmlnode,iedName,apName,ldInst,cbName)
              if(dateset=={}):
                print('Warnning! appid: 0x'+appid+' dataset unknow.')    
              appiddict[appid]={'iedName':iedName,'apName':apName,'cbName':cbName,'ldInst':ldInst,'mac':mac,'vlanid':vlanid,'vlanpriority':vlanpriority,'dataset':dateset}
  return(appiddict)

def main(xmlfile):
  iedexample='CL2201A'
  gseexample='20'
  smvexample='4008'
  datasetexample='dsEvnInfo'
  rt=0
  root=(xml_parse.openxml(xmlfile))
  ieddescdict=getieddesc(root)
  print(iedexample,ieddescdict[iedexample])
  gseappiddict=getappid(root,'GSE')
  print(gseexample,gseappiddict[gseexample])
  print('92',gseappiddict['92'])
  smvappiddict=getappid(root,'SMV')
  print(smvexample,smvappiddict[smvexample])
  return(rt)

if(__name__=='__main__'):
  xmlfile='/mnt/e/ts/scd/fengqiao220_08101419.scd'
  main(xmlfile)
  