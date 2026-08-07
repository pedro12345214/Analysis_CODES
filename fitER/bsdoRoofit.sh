DOANALYSISPbPb_FULL_BS=1
DOANALYSISPbPb_BINNED_PT_BS=0
DOANALYSISPbPb_BINNED_Y_BS=0
DOANALYSISPbPb_BINNED_MULT_BS=0

#Data and MC Samples ppRef
#Data_Bs="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bs_pp24_v3_fid2_14v1_xgb_v1/DATA_with_score.root"
#MC_Bs="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bs_pp24_v3_fid2_14v1_xgb_v1/MC_with_score.root"
#Data and MC Samples PbPb
Data_Bs="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bs_pb24_v2_fid1_14v1_xgb_v1/DATA_with_score.root"
MC_Bs="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bs_pb24_v2_fid1_14v1_xgb_v1/MC_with_score.root"

## NEW CUTS ? here 
#CUTs="Bnorm_svpvDistance_2D > 4"
#CUTs="Prediction > 0.77 && abs(By) < 2.4 && Bpt > 7.5 && Bnorm_svpvDistance_2D > 2 && BtrkPtimb < 0.2 && Bchi2Prob > 0.02" #pp
CUTs="Prediction > 0.68 && abs(By) < 2.4 && Bpt > 7.5 && Bnorm_svpvDistance_2D > 2 && BtrkPtimb < 0.2 && Bchi2Prob > 0.02" #pb

##
#syst="ppRef"
syst="PbPb"

mkdir -p ROOTfiles/

#The Function to be called:
#
#void roofitB(TString TREE = "ntphi", int FULL = 0, TString INPUTDATA = "", TString INPUTMC = "", TString VAR = "", TString CUT = "", TString SYSTEM = "ppRef"){


if [ $DOANALYSISPbPb_FULL_BS  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntphi\", \
                      1, \
                      \"$Data_Bs\", \
                      \"$MC_Bs\", \
                      \"Bpt\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi

if [ $DOANALYSISPbPb_BINNED_PT_BS  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntphi\",\
                      0, \
                      \"$Data_Bs\", \
                      \"$MC_Bs\", \
                      \"Bpt\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi

if [ $DOANALYSISPbPb_BINNED_Y_BS  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntphi\",\
                      0, \
                      \"$Data_Bs\", \
                      \"$MC_Bs\", \
                      \"By\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi

if [ $DOANALYSISPbPb_BINNED_MULT_BS  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntphi\",\
                      0, \
                      \"$Data_Bs\", \
                      \"$MC_Bs\", \
                      \"nSelectedChargedTracks\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi

rm roofitB_C.d roofitB_C_ACLiC_dict_rdict.pcm roofitB_C.so
