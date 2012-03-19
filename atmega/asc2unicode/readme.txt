unsigned char packet[]; 
 unsigned char i=0; 
 unsigned char qm;//区码 
 unsigned char wm;//位码 
 unsigned int hz; 
 unsigned char buffer[]="汉字"; 

 if(*buffer>=0xa0)//汉字在计算机内部为负，即最高位为1，一般作判断大于0xa0即可 
  {                //判断是汉字转为unicode码	    
   qm=*buffer-0xa0;//由ascii转化为 区码和位码 
   buffer++; 
   wm=*buffer-0xa0; 
   buffer++; 
   qm=qm-0x10;//区码偏移量处理 
   hz=Unicode[qm][wm];//根据区位码查出unicode码。 
   packet[i++]=(unsigned char)(hz>>8);	   
   packet[i++]=(unsigned char)(hz & 0x00ff);	   
  } 
  else//不是汉字而是西文字符转化为unicode 
  { 
   packet[i++]= 0x00;	   
   packet[i++]= *buffer;	  
   buffer++; 
  } 


http://www.ourdev.cn/bbs/bbs_content.jsp?bbs_sn=4990553&bbs_page_no=1&bbs_id=1000