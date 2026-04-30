#include "dominance.h"
#include "tabNDS.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*POUR LA MAXIMISATION*/

int* result;
double* sol1T;


int dominatePareto(int *sol1,int *sol2, int nbCrit){ 
  //we compare sol1 to sol2, minimisation of all criteria
  //Domine = 1, Dominated = 2, Equivalent = 3, Equal = 4}
  
  if (nbCrit==2){
    if (((sol1[0]>sol2[0]) && (sol1[1]>=sol2[1])) || ((sol1[0]>=sol2[0]) && (sol1[1]>sol2[1])))
      return 1;
    if ((sol1[0]==sol2[0]) && (sol1[1]==sol2[1]))
      return 4;
    if (((sol1[0]<sol2[0]) && (sol1[1]<=sol2[1])) || ((sol1[0]<=sol2[0]) && (sol1[1]<sol2[1])))
      return 2;
    return 3;
  }else{
     int ctrSup=0;
     int ctrInf=0;
     int ctrEgal=0;
     int i=0;
     while (((ctrSup==0) || (ctrInf==0)) && (i<=nbCrit-1)){
       if (sol1[i] < sol2[i]){
         ctrInf=ctrInf+1;
         if (ctrSup>0)  
           return 3;
       } 
       else
         if (sol1[i] > sol2[i]){ 
           ctrSup=ctrSup+1;
           if (ctrInf>0) 
             return 3;
         }
         else
           ctrEgal=ctrEgal+1;
       i=i+1;
     }
     if ((ctrInf>0) && (ctrSup==0))
       return 2;
     if ((ctrSup>0) && (ctrInf==0))
       return 1;
     if (ctrEgal==nbCrit)
       return 4;
     if ((ctrSup>0) && (ctrInf>0)) //normally not useful
       return 3;
    
  }     
  return -1; 
}   

int dominateParetoDouble(double *sol1,double *sol2, int nbCrit){ /*toujours en minimisation*/
  //we compare sol1 to sol2, minimisation of all criteria
  //Domine = 1, Dominated = 2, Equivalent = 3, Equal = 4}
  double eps=0.001;
 
  int ctrSup=0;
  int ctrInf=0;
  int ctrEgal=0;
  int i=0;
  while (((ctrSup==0) || (ctrInf==0)) && (i<=nbCrit-1)){
     if ((fabs(sol1[i]-sol2[i])<eps)){ 
         ctrEgal=ctrEgal+1;
     }
     else{  
       if (sol1[i] < sol2[i]){
         ctrInf=ctrInf+1;
         if (ctrSup>0)  //A VOIR
           return 3;
       } 
       else{
         if (sol1[i] > sol2[i]){ 
           ctrSup=ctrSup+1;
           if (ctrInf>0) 
             return 3;
         }
       }
     }
     i=i+1;
   }
   if ((ctrInf>0) && (ctrSup==0))
     return 1;
   if ((ctrSup>0) && (ctrInf==0))
     return 2;
   if (ctrEgal==nbCrit)
     return 4;
   if ((ctrSup>0) && (ctrInf>0)) //normally not useful
     return 3;
         
  return -1; 
}   
  
