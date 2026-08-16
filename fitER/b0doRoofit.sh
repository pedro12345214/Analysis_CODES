DOANALYSISPbPb_FULL_B0=0
DOANALYSISPbPb_BINNED_PT_B0=1
DOANALYSISPbPb_BINNED_Y_B0=0
DOANALYSISPbPb_BINNED_MULT_B0=0

#Data and MC Samples ppRef
#Data_B0="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bd_pp24_v1_fid1_14v1_xgb_v1/DATA_with_score.root"
#MC_B0="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bd_pp24_v1_fid1_14v1_xgb_v1/MC_with_score.root"
#Data and MC Samples PbPb
Data_B0="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bd_pb24_v1_fid1_14v1_xgb_v1/DATA_with_score.root"
MC_B0="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bd_pb24_v1_fid1_14v1_xgb_v1/MC_with_score.root"

## CUTS (SELECTION ?) here 
#CUTs="Bnorm_svpvDistance_2D > 4"
#CUTs="Prediction > 0.92 && abs(By) < 2.4 && Bpt > 7.5 && Bnorm_svpvDistance_2D > 2 && BtrkPtimb < 0.2 && Bchi2Prob > 0.02" 
CUTs="Prediction > 0.93 && abs(By) < 2.4 && Bpt > 7.5 && Bnorm_svpvDistance_2D > 2 && BtrkPtimb < 0.2 && Bchi2Prob > 0.02" #pb

##
#syst="ppRef"
syst="PbPb"

mkdir -p ROOTfiles/

#The Function to be called:
#
#
#void roofitB(TString TREE = "ntphi", int FULL = 0, TString INPUTDATA = "", TString INPUTMC = "", TString VAR = "", TString CUT = "", TString SYSTEM = "ppRef"){


if [ $DOANALYSISPbPb_FULL_B0  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntKstar\", \
                      1,\
                      \"$Data_B0\", \
                      \"$MC_B0\", \
                      \"Bpt\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi

if [ $DOANALYSISPbPb_BINNED_PT_B0  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntKstar\", \
                      0, \
                      \"$Data_B0\", \
                      \"$MC_B0\", \
                      \"Bpt\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi

if [ $DOANALYSISPbPb_BINNED_Y_B0  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntKstar\",\
                      0, \
                      \"$Data_B0\", \
                      \"$MC_B0\", \
                      \"By\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi

if [ $DOANALYSISPbPb_BINNED_MULT_B0  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntKstar\",\
                      0, \
                      \"$Data_B0\", \
                      \"$MC_B0\", \
                      \"nSelectedChargedTracks\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi


rm roofitB_C.d roofitB_C_ACLiC_dict_rdict.pcm roofitB_C.so
