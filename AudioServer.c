//                                  `.:/`           `:+sysmmddhyso+/++oossssss-  
//                               ./oooo//.` `.:/+osyyyyyssyyysssoo+/:/oydMh+.````
//                          `/ooo/..   -/osys+/:--.``                    `/sdh/` 
//                      `:ss+``    -+syo:``                                  .ody:
//                   `:ss/.    -+yds:`                                         `:yd/    
//                `/yy/`    `+dds-`                                               .sd:  
//              `+ho-     :yho-`                                                    -hs`
//            `+h+`     /ds-`                                                        `om/     
//          `+my.     /dh-                                            -/o:             -ds    
//         .dd:     :hd:                                          -++hho--.- `          `hh`  
//        /mo`    .ym+`                                    . `-:smNMm++ymNNNdNh`         `yd` 
//       /d-     :do`                                  `:odhhmdmMMMMNNMMMMMMMMMs          `yd`
//      +m-    `sd-                                 -/ymMMMMMMMMMMMMMMdy//dMMMMM/          `ds
//     :N/    -dd.                              ./smNMMMMMMMMMMMMMMNo-`   .MMMMMN`          -N. 
//    .mo    :Nh`                           ./ymNMMMMMMMMMMMMMMNMMMMm:     dMMMMMs           ss 
//    hy    /Ny`                        `:odNMMMMMMMMMMMMMMMmy:-NMMMMNo`   -NMMMMd           :N`
//   /N`   :Nh`                      ./ymMMMMMMMMMMMMMMMMMy:`   +MMMMMMd    -mMMMd           .Mo
//   ds   -NN.                  `-/yhmMMMMMNMMMMMMMMMMMMMMm-     oMMMMMM-    -NMMs           `Mo
//  :M.  `dM+              `.-/sdNMMMMMMmddNMMMMMMMMMMMMMMMN+     +NMMNd`     -/-`            M/
//  yd   sMd          `-/sdmmNMMMMMMMMMMmMMMMMMs:dMMMMMMMMMMMo     ./:-`                     `M:
//  N/  `NM-       -/ydNMMMMMMMMMMMMMMMMNNMMMMM` :MMMMMMMMMMMs                               /M`
// -M`  oMM     .+hNMMMMMMMMMMMMMMMMMmNN--MMMMMo  dMMMMMMMNmy`                               yh 
// oM   hMN  .+hNMMMMMMMMMMMMMmNMMMMM++m  hMMMMN. .dMMMMMM/.                                 N/ 
// ym   dMM:yNMMMMMMNNMMMMMMMy..dMMMMM/M` -NMMMMy  `dMMMMMd`                                +d` 
// yh   dMMMMMMMMNh/.-NMMMMMMy  .NMMMMyN:  :dNMm+   :MMMMMMo                               -m.  
// yy  :MMMMMMmmNy    /MMMMMMM`  :MMMMNN+    /N`     NMMMMMM:                             :d:
// oy  oNNMMM+. oh     sMMMMMM/   omMy-oy     h+    `dMMMMMMm                           .yh. 
// +y   .:sNMd- `m-    `NmdNNNy    `d+ :N     /N.:+ssmMMMMMMM/                        `oms`
// -N      :dMNo`/d     /d ../N`    +d `M.`.:+yMm/-./dMMMMMMMm                      `+do. 
//  N.       /dMm+ho    `N-   m+    -M/oMds+/--ymoys/-mMMMMMMM/                  ./sho.
//  y+        `/mMNN.    ys   sd`./oyMd/md/+osssMo`   oMMMMMMMy              `.+yh+-
//  :d          `oNMd-   /m  .yMhs/--hNhmM/-`   dy    :MMMMMMMm           `.+yy/.
//   d.           .yMNs. +MoyyyM/`/yhoN+-M.     /N`    MMMMMMMM`       `-ohy/`
//   /y             :yMNhMN+` +Mdh+.  yy N/      N/    dMMMMMMMs`  ``/oys:` 
//    y-              /sNMs:oyNMs     -M`sy      oh    yMMMMMMMM/:ohdo-
//    `/               `yNNMM+`No      d+.d      .M.   /MMMMMMMMds/`   
//                      ` `/hMdNh      /m         hs   .MMMMMMM/
//                           .+dMms:.   /`        .o ./dMMM+mMm`
//                              `/shNmhysoo++++osyhdhy+::+./my. 
//                                   `.-://++//:-.`

// Audio TCP Server For the Dafang running modded firmware
// For NurdSpace
// By Melan - 2020


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <imp/imp_audio.h>
#include <imp/imp_log.h>

// For socket server
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#define AEC_SAMPLE_RATE 8000
#define AEC_SAMPLE_TIME 10

#define IMP_AUDIO_BUF_SIZE (5 * (AEC_SAMPLE_RATE * sizeof(short) * AEC_SAMPLE_TIME / 1000))



int  main(int argc, char *argv[])
{
	int chnVol = 50;
	int hSocket;
	int ret;

    if (argc > 1 && argv[1] != NULL) chnVol = atoi(argv[1]);

	// Setup socket
	hSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server = {0};
	server.sin_family = AF_INET;
	server.sin_port = htons(9000);
	server.sin_addr.s_addr = INADDR_ANY;

	// Bind
	bind(hSocket, (struct sockaddr*) &server, sizeof(server));
	listen(hSocket, 5);


	/* Step 1: set public attribute of AO device. */
	int devID = 0;
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_8000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 400;
	attr.chnCnt = 1;

	ret = IMP_AO_SetPubAttr(devID, &attr);
	if(ret != 0) fprintf(stderr, "set ao %d attr err: %d\n", devID, ret); return NULL;

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AO_GetPubAttr(devID, &attr);
	if(ret != 0) fprintf(stderr, "get ao %d attr err: %d\n", devID, ret); return NULL;

	
	/* Step 2: enable AO device. */
	ret = IMP_AO_Enable(devID);
	if(ret != 0) fprintf(stderr, "enable ao %d err\n", devID); return NULL;

	/* Step 3: enable AI channel. */
	int chnID = 0;
	ret = IMP_AO_EnableChn(devID, chnID);

	if(ret != 0) fprintf(stderr, "Audio play enable channel failed\n"); return NULL;

	/* Step 4: Set audio channel volume. */
	ret = IMP_AO_SetVol(devID, chnID, chnVol);
	if(ret != 0) fprintf(stderr, "Audio Play set volume failed\n"); return NULL;

	ret = IMP_AO_GetVol(devID, chnID, &chnVol);
	if(ret != 0) fprintf(stderr, "Audio Play get volume failed\n"); return NULL;

	fprintf(stderr, "Audio Out GetVol    vol:%d\n", chnVol);

	ret = IMP_AO_EnableHpf(&attr);
	if(ret != 0) fprintf(stderr, "enable audio hpf error.\n"); return NULL;

	// Listen for new clients
	char buf[IMP_AUDIO_BUF_SIZE];	
	while (1) {
		int client;
		client = accept(hSocket, NULL, NULL);
		fprintf(stderr, "Client connected\n");

		//read data into buffer
		while(recv(client, buf, sizeof(buf), MSG_PEEK) != 0) {
			int rc = read(client, buf, sizeof(buf));
			
			// Play audio?
			if (rc != 0) {
				/* Step 5: send frame data. */
				IMPAudioFrame frm;
				frm.virAddr = (uint32_t *)buf;
				frm.len = rc;
				IMP_AO_SendFrame(devID, chnID, &frm, BLOCK);
			}
		}	
		fprintf(stderr, "Disconnecting client.\n");
		close(client);
	}

	close(hSocket);
	return 0;
}
