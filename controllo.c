void procControllo (int *pipe_fds){
	close(pipe_fds[1]);
	Coccodrillo coccodrilli[8];
	Proiettile proiettili[10];
	for(int i=0;i<MAX_COCCODRILES;i++){
		coccodrilli[i].id=i;
	}
	for(int i=8;i<18;i++){
		proiettili[i].id=i;
	}
	
	Temp temp;
	vite=3;
	int n_coccodrilli;
	int id_morti[8];
    	int n_flusso;
    	Flusso flussi[8];
   	definizione_flussi(flussi);
   	
   		
	while(vite>0){
		
		creazione_processi(n_coccorilli, id_morti , flussi, coccodrilli);
		
		read(pipe_fds[0], &temp, sizeof(Temp));
		
		
	
	
	
	}
	