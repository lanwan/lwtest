unsigned char packet[]; 
 unsigned char i=0; 
 unsigned char qm;//���� 
 unsigned char wm;//λ�� 
 unsigned int hz; 
 unsigned char buffer[]="����"; 

 if(*buffer>=0xa0)//�����ڼ�����ڲ�Ϊ���������λΪ1��һ�����жϴ���0xa0���� 
  {                //�ж��Ǻ���תΪunicode��	    
   qm=*buffer-0xa0;//��asciiת��Ϊ �����λ�� 
   buffer++; 
   wm=*buffer-0xa0; 
   buffer++; 
   qm=qm-0x10;//����ƫ�������� 
   hz=Unicode[qm][wm];//������λ����unicode�롣 
   packet[i++]=(unsigned char)(hz>>8);	   
   packet[i++]=(unsigned char)(hz & 0x00ff);	   
  } 
  else//���Ǻ��ֶ��������ַ�ת��Ϊunicode 
  { 
   packet[i++]= 0x00;	   
   packet[i++]= *buffer;	  
   buffer++; 
  } 


http://www.ourdev.cn/bbs/bbs_content.jsp?bbs_sn=4990553&bbs_page_no=1&bbs_id=1000