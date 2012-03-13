#include "a_laudo.h"


//float get_baseline(float y[])
//{
//  float sum=0;
//  int i;
//
//  // Compute the base line for the first 10% vector points
//  for(i=0; i<EVENT_SIZE/10; i++)
//    {
//      sum = sum+y[i];
//    }
//  sum = (sum*10)/EVENT_SIZE;
//  return sum;
//}
//
//float get_amplitude(int size, float base, float y[], bool fbslope)
//{
//  int i;
//  float max = -1E10;
//  float min = 1E10;
//  float ch_max, ch_min, ch_thresh, amp;
//
//  // Get max and min
//  for(i=0; i<size; i++)
//  {
//    if(y[i]>max)
//	{
//	  max = y[i];
//	  //ch_max = i*TIMEBIN; //16.666666;
//	}
//    
//	if(y[i]<min)
//	{
//	  min = y[i];
//	  //ch_min = i*TIMEBIN; //16.666666;
//	}
// }
//
///*
//  i=0;
//  while(y[i]>min/10.)
//    {
//      ch_thresh=i*TIMEBIN; //16.666666;
//      i++;
//    }
//*/
//
//  // Take polarity into account
//  if(fbslope==0) // 0 - Negative
//    {
//      amp = min - base;
//      // amp = -amp;
//      // cout << base << " " << ch_thresh << " " << ch_min << endl;
//      // cout << base << " " << amp << endl;
//    }
//  else // 1 - Positive
//    {
//      amp = max - base;
//      //if(amp>10)
//      //  cout << base << " " << amp << " " << ch_max << endl;
//    }
//  return amp;
//}
//
//float get_charge(int size, float base, float y[], bool fbslope)
//{
//  int i;
//  float q=0;
//
//  for(i=0; i<size; i++)
//    q = q + (y[i]-base);
//
//  q = (q*TIMEBIN)/50.; //Charge in pico-coulomb
//
//  if(fbslope==0) // 0 is Negative
//    q = -q;
//
//  return q;
//}
