DOANALYSISPbPb_FULL_Bp=0
DOANALYSISPbPb_BINNED_PT_Bp=1
DOANALYSISPbPb_BINNED_Y_Bp=0
DOANALYSISPbPb_BINNED_MULT_Bp=0

#Data and MC Samples ppRef
Data_Bp="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bu_pp24_v1_fid1_10v1_xgb_v1/DATA_with_score.root"
MC_Bp="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bu_pp24_v1_fid1_10v1_xgb_v1/MC_with_score.root"
#Data and MC Samples PbPb
#Data_Bp="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bu_pb24_v1_fid1_10v1_xgb_v1/DATA_with_score.root"
#MC_Bp="/lstore/cms/hlegoinha/RUN3_Data_MC_sharing/Bmesons/Bu_pb24_v1_fid1_10v1_xgb_v1/MC_with_score.root"

## NEW CUTS ? here 
#CUTs="Bnorm_svpvDistance_2D > 4"
#ppRef Cut+Preselection
CUTs="(abs(By) < 2.4) && (Bpt > 7.5) && (Prediction > 0.98)" 
#pb Cut+Preselection
#CUTs= "Prediction > 0.98 && abs(By) < 2.4 && Bpt > 7.5" 

##
syst="ppRef"
#syst="PbPb"

mkdir -p ROOTfiles/

#The Function to be called:
#
#void roofitB(TString TREE = "ntphi", int FULL = 0, TString INPUTDATA = "", TString INPUTMC = "", TString VAR = "", TString CUT = "", TString SYSTEM = "ppRef"){



if [ $DOANALYSISPbPb_FULL_Bp  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntKp\", \
                      1, \
                      \"$Data_Bp\", \
                      \"$MC_Bp\", \
                      \"Bpt\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi

if [ $DOANALYSISPbPb_BINNED_PT_Bp  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntKp\",\
                      0, \
                      \"$Data_Bp\", \
                      \"$MC_Bp\", \
                      \"Bpt\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi

if [ $DOANALYSISPbPb_BINNED_Y_Bp  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntKp\",\
                      0, \
                      \"$Data_Bp\", \
                      \"$MC_Bp\", \
                      \"By\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi

if [ $DOANALYSISPbPb_BINNED_MULT_Bp  -eq 1  ]; then
root -b -q "roofitB.C++(\"ntKp\",\
                      0, \
                      \"$Data_Bp\", \
                      \"$MC_Bp\", \
                      \"nSelectedChargedTracks\", \
                      \"$CUTs\", \
                      \"$syst\")"
fi


rm roofitB_C.d roofitB_C_ACLiC_dict_rdict.pcm roofitB_C.so
