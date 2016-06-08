//
//  main.cpp
//  data_flow_test
//
//  Created by huagao on 2/1/16.
//  Copyright © 2016 huagao. All rights reserved.
//

#include "stdio.h"
#include "echo_control_mobile.h"
#include "WaveIO.h"
#define FRAME_IN_TIME_MS 10



int main(int argc, const char * argv[]) {
    // insert code here...
    
    const char * read_file_name1 = NULL;
    const char * read_file_name2 = NULL;
    const char * write_file_name = NULL;
   
    if(argc<4)
    {
        printf("Not Enough Input Parameters!");
        return 0;
    }
  //  read_file_name1 = argv[1];
  //  read_file_name2 = argv[2];
  //  write_file_name = argv[3];
    //s6
    read_file_name1 = "/Users/huagao/works/sample/with_near_end_5cm_to_MAC/input/near16_com.wav";
    read_file_name2 = "/Users/huagao/works/sample/with_near_end_5cm_to_MAC/input/far16.wav";
    write_file_name = "/Users/huagao/works/sample/with_near_end_5cm_to_MAC/input/webrtcfloat_out_fix64.wav";
    //s5
    /*read_file_name1 = "/Users/huagao/works/sample/s5/AECNearInIutput-float32-Chn1-16000.wav";
    read_file_name2 = "/Users/huagao/works/sample/s5/AECFarInIutput-float32-Chn1-16000.wav";
    write_file_name = "/Users/huagao/works/sample/s5/webrtc_out_float.wav";
    */
     SWavFileHead wavhead1 = {0};
     SWavFileHead wavhead2 = {0};
     CWavFileOp *read_file1 = NULL;
     CWavFileOp *read_file2 = NULL;
     CWavFileOp *write_file1 = NULL;
     CWavFileOp *write_file2 = NULL;
     unsigned int frame_size = 0;
     unsigned char * buffer =NULL; // hold readout from file
     unsigned char * processing_buffer = NULL ; // hold type of input request
     short * process_in =NULL;
    short * reffer_in = NULL;

     read_file1 = new CWavFileOp((char*)read_file_name1,"r");
     if (read_file1->m_FileStatus==-2)
     {
     delete read_file1;
     printf("Can not open the 1st Input wav file!");
     return 0;
     }
     
     read_file2 = new CWavFileOp((char*)read_file_name2,"r");
     if (read_file2->m_FileStatus==-2)
     {
     delete read_file2;
     printf("Can not open the 2nd Input wav file!");
     return 0;
     }
     write_file1=new CWavFileOp((char*)write_file_name,"wb");
     if (write_file1->m_FileStatus==-2)
     {
     delete write_file1;
     printf("Can not open the 1s Output wav file!");
     return 0;
     }
     ///init about file
     read_file1->ReadHeader(&wavhead1);
     frame_size = wavhead1.SampleRate * FRAME_IN_TIME_MS /1000;
     read_file2->ReadHeader(&wavhead2);
    if( (wavhead1.SampleRate != wavhead2.SampleRate)
       ) {
        return 0;
    }
     //write a exist head
     write_file1->WriteHeader(wavhead1);
     
     //init buffer
     buffer = new unsigned char[2* frame_size * wavhead1.BytesPerSample+2* frame_size * wavhead2.BytesPerSample];
     processing_buffer = buffer + frame_size * wavhead1.BytesPerSample;
    unsigned char* buffer2 = processing_buffer + frame_size * wavhead1.BytesPerSample;
    unsigned char* refer_buffer = buffer2 + frame_size * wavhead2.BytesPerSample;
     memset(buffer, 0, 2* frame_size * wavhead1.BytesPerSample+2* frame_size * wavhead2.BytesPerSample);
     ///init processing
    void* aecmInst =WebRtcAecm_Create();
    if ( 0 != WebRtcAecm_Init(aecmInst, wavhead1.SampleRate)) {
        return 0;
    }
     ////
     int loop =0;
     for (loop = 0 ; loop  < wavhead1.RawDataFileLength; loop += frame_size * wavhead1.BytesPerSample) {
         if (loop >658621*wavhead1.BytesPerSample) {
             loop*=1;
         }
         read_file1->ReadSample(buffer, frame_size);
         if (3 == wavhead1.FormatCategory) {
             
             float * psi =(float * )buffer;
             short * pso = (short *)processing_buffer;
             for (int i = 0; i< frame_size; i++) {
                int j = i * wavhead1.NChannels;
                 if (32766.9f<psi[j]) {
                     pso[i]=32767;
                 }
                 else if (-32767.9f>psi[j]){
                     pso[i]= -32768;
                 }
                 else
                     pso[i]=static_cast<short>(psi[j]);
             }
             process_in =pso;
         }
         else if (1 == wavhead1.FormatCategory) {
             
             short * pso = (short *)processing_buffer;
             short * psi =(short * )buffer;
             for (int i = 0; i< frame_size ; i++) {
                 pso[i]= psi[i * wavhead1.NChannels];
             }
             process_in =pso;
         }
         
     //////////
          read_file2->ReadSample(buffer2, frame_size);
         if (3 == wavhead2.FormatCategory) {
             
             float* psi =(float* )buffer2;
             short * pso = (short *)refer_buffer;
             for (int i = 0; i< frame_size; i++) {
                 int j = i * wavhead2.NChannels;
                 if (32766.9f<psi[j]) {
                     pso[i]=32767;
                 }
                 else if (-32767.9f>psi[j]){
                     pso[i]= -32768;
                 }
                 else
                     pso[i]=static_cast<short>(psi[j]);
             }
             reffer_in =pso;
         }
         else if (1 == wavhead2.FormatCategory) {
             
             short * pso = (short *)refer_buffer;
             short * psi =(short * )buffer2;
             for (int i = 0; i< frame_size ; i++) {
                 pso[i]= psi[i * wavhead2.NChannels];
             }
             reffer_in =pso;
         }
//////////////
         WebRtcAecm_BufferFarend( aecmInst,
                                 reffer_in,
                                 frame_size);
         WebRtcAecm_Process(aecmInst,
                            process_in,
                            NULL,
                            (short *)buffer,
                            frame_size,
                            0);
         /////
  
         write_file1->WriteSample((short *)buffer, frame_size);
     
     }
   
     //flash head
     write_file1->UpdateHeader_new(1, wavhead1.SampleRate);
     //close wave file
     if (NULL != write_file1) {
     delete write_file1;
     write_file1 = NULL;
     }
     //close wave file
     if (NULL != read_file1) {
     delete read_file1;
     read_file1 = NULL;
     }
     

     return 0;
    
    
}

