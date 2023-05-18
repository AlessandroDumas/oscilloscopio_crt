
# prende l'input stereo S16_LE da jack
# e lo invia al programma 
# poi disegna il "buffer" a schermo con ffplay

size=768x512
size=640x512

jack-stdout -L -S 512 system:capture_1 system:capture_2 | ./main | ffplay -f rawvideo -pix_fmt gray16le -s:v $size -i -

