# ratkiewicz1@llnl.gov
# this script reads a lot of histos with a IC spectra, finds the centroids
# and calculates the gain shift necessary to put them on a common scale.
# IC is only available as a 2D plot, so will need to project it

from matplotlib import pyplot as plt
import numpy as np
import os,random
from ROOT import TFile,TCanvas,TH2D,TSpectrum

#plt.style.use(['fivethirtyeight','paper538'])

#HIST_BASE = '/Users/ratkiewicz1/Desktop/goddess_2019/134Te/hists'
HIST_BASE = '/global/data1b/gretina/1484_data1b/Histograms/8Apr_pareddown'

def getAllHists(fName,flag='',exclude='',folder=''):
    fR = TFile.Open(fName,'READ')
    if len(folder) != 0:
        fR.cd(folder)
    hList = []
    for key in fR.GetListOfKeys():
        if len(flag) == 0:
            h = fR.Get(key.GetName())
            h.SetDirectory(0)
            hList.append(h)
        else:
            if len(exclude) == 0:
                if flag in key.GetName():
                    h = fR.Get(key.GetName())
                    h.SetDirectory(0)
                    hList.append(h)
            else:
                if flag in key.GetName() and exclude not in key.GetName():
                    h = fR.Get(key.GetName())
                    h.SetDirectory(0)
                    hList.append(h)
    
    # when we open TFile we change gDirectory to point to it.
    # then the histogram is associated with the file, so when the
    # function returns -- whether or not we call fR.Close() --
    # the histogram disappears. So we call SetDirectory(0) to
    # disassociate the histogram with fR.

    #fR.Close()


    return hList

def run_hists(hist_dir,include_name,exclude_name,hist_name,out_name):
    # makes a file will all the specified histos from all the runs in it
    fListRaw = sorted([os.path.join(hist_dir,f) for f in os.listdir(hist_dir) if 
                os.path.isfile(os.path.join(hist_dir,f)) and
                include_name in os.path.basename(f) and 
                os.path.splitext(f)[1] == '.root'])
    fDict = {}
    for f in fListRaw:
        iF = int(os.path.splitext(os.path.basename(f))[0].replace('hist','').replace('bkgd_',''))
        fDict[iF] = f

    fList = []
    for key in sorted(fDict.keys()):
        fList.append(fDict[key])

    hD = {}
    if '.root' not in out_name:
        out_name = '%s.root'%(out_name)

    for f in fList:
        hD[f] = getAllHists(f,hist_name,exclude_name)
        # get uID for the file

    fW = TFile.Open(os.path.join(hist_dir,out_name),'RECREATE')
    for f in fList:
        uid = os.path.splitext(os.path.basename(f))[0]
        for h in hD[f]:
            hN = h.GetName()
            h.SetName('%s_%s'%(uid,hN))
            h.Write()

def make_projections(fName,hName):
    hD = getAllHists(fName,hName)
    projX,projY = [],[]

    for h in hD:
        hX = h.ProjectionX('%s_px'%(h.GetName()))
        hY = h.ProjectionY('%s_py'%(h.GetName()))
        projX.append(hX)
        projY.append(hY)

    return projX,projY

def make_summary_spectra(hList,hName,hTitle,runL=[],gainL=[]):
    xHD = [len(hList)+1,0,len(hList)-1] # x is num runs: bins, low end, high end
    if len(gainL) == 0:
        yHD = [hList[0].GetNbinsX(),
                hList[0].GetXaxis().GetXmin(),
                hList[0].GetXaxis().GetXmax()]

    else:
        yRaw = [hList[0].GetNbinsX(),
                hList[0].GetXaxis().GetXmin(),
                hList[0].GetXaxis().GetXmax()]

        hSumRaw = TH2D('%s_raw'%(hName),'%s_raw'%(hTitle),
                        xHD[0],xHD[1],xHD[2],
                        yRaw[0],yRaw[1],yRaw[2])


        yHD = [hList[0].GetNbinsX(),
                hList[0].GetXaxis().GetXmin()*min(gainL),
                hList[0].GetXaxis().GetXmax()*max(gainL)]

    hSum = TH2D(hName,hTitle,xHD[0],xHD[1],xHD[2],yHD[0],yHD[1],yHD[2]) 

    hAX = hSum.GetYaxis()
    for i,h in enumerate(hList):
        hIdx = h.GetName()

        if 'bkgd' not in hIdx:
            hIdx = hIdx.replace('hist','').replace('_IC_EdE_p','')
            hIdx = int(hIdx.replace('x','').replace('y',''))

            for j in range(0,yHD[0]):
                if len(gainL) == 0:
                    hSum.SetBinContent(i+1,j,hList[i].GetBinContent(j))
                else:
                    calIdx = runL.index(hIdx)
                    tJ = hAX.FindBin(hAX.GetBinCenter(j)*gainL[calIdx])
                    hSum.SetBinContent(i+1,tJ,hList[i].GetBinContent(j))

    return hSum

#if not os.path.isfile(os.path.join(HIST_BASE,'all_IC_EdE.root')):
run_hists(HIST_BASE,'hist','bkgd','IC_EdE','all_IC_EdE')

pX,pY = make_projections(os.path.join(HIST_BASE,'all_IC_EdE.root'),'IC_EdE')

# make summary spectra
sDE = make_summary_spectra(pX,'ic_dE_raw','ic dE summary')
sE = make_summary_spectra(pY,'ic_E_raw','ic E summary')

# now find the peaks and plot them
s = TSpectrum(10)
r = []
pDE,pE = [],[]
for h in pX:
    if 'bkgd' not in h.GetName():
        h.SetAxisRange(1000,3000,'X')
        nFound = s.Search(h,2,"goff",0.05)
        xpeak = s.GetPositionX()[0]
        hK = int((h.GetName()).replace('hist','').replace('_IC_EdE_px',''))
        r.append(hK)
        pDE.append(xpeak)
for h in pY:
    if 'bkgd' not in h.GetName():
        h.SetAxisRange(1000,1500,'X')
        nFound = s.Search(h,2,"goff",0.05)
        xpeak = s.GetPositionX()[0]    
        pE.append(xpeak)
    
#pDE = [x for _,x in sorted(zip(r,pDE))]
#pE = [x for _,x in sorted(zip(r,pE))]
#r = sorted(r)

ic_de_ref = 2215.0
ic_e_ref = 720.0

c_pDE = [ic_de_ref/ic for ic in pDE]
c_pE = [ic_e_ref/ic for ic in pE]

with open('ic_cor_tmp.dat','w') as outFile:
    outFile.write('#run_num\tdE_cor\tE_cor\n')
    outFile.write('# IC_dE ref: %.02f\n'%(ic_de_ref))
    outFile.write('# IC_E ref : %.02f\n'%(ic_e_ref))

    for i,rN in enumerate(r):
        outFile.write('%i\t%.04f\t%.04f\n'%(rN,
                        ic_de_ref/pDE[i],
                        ic_e_ref/pE[i],))

hSC_dE = make_summary_spectra(pX,'ic_dE_cor','ic dE summary cor',r,c_pDE)
hSC_E = make_summary_spectra(pY,'ic_E_cor','ic E summary cor',r,c_pE)


"""
fig = plt.figure()
ax = plt.axes()

plt.step(r,pDE/np.mean(pDE),label='IC dE')
plt.step(r,pE/np.mean(pE),label='IC E')
ax.set_xlabel('run number')
ax.set_ylabel('centroid of most intense peak div. mean value')
ax.legend(loc='best')

plt.savefig('images/ic_cents.png',bbox_inches='tight')

plt.show()
"""

c1 = TCanvas()

sDE.Draw('colz')
sDE.SetYTitle('IC dE (abs)')
sDE.SetXTitle('run (abs)')

c1.SetLogz()
c1.Modified()
c1.Update()
c1.SaveAs('images/IC_dE.root')

raw_input('press enter')


sE.Draw('colz')
sE.SetAxisRange(10,5000,'Y')
sE.SetYTitle('IC E (abs)')
sE.SetXTitle('run (abs)')

c1.SetLogz()
c1.Modified()
c1.Update()
c1.SaveAs('images/IC_E.root')

raw_input('press enter')

hSC_dE.Draw('colz')
hSC_dE.SetYTitle('IC dE (abs)')
hSC_dE.SetXTitle('run (abs)')

c1.SetLogz()
c1.Modified()
c1.Update()

c1.SaveAs('images/IC_dE_cor.root')
raw_input('press enter')

hSC_E.Draw('colz')
#hSC_E.SetAxisRange(0,2000,'Y')
hSC_E.SetYTitle('IC E (abs)')
hSC_E.SetXTitle('run (abs)')
c1.SetLogz()
c1.Modified()
c1.Update()

c1.SaveAs('images/IC_E_cor.root')
raw_input('press enter')


#fig,ax = plt.subplots(figsize=(10,6))

