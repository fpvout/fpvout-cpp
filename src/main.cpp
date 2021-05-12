#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libusb.h>
#include <err.h>
#include <sys/timeb.h>

using namespace std;
using namespace std::chrono;

double now() {
    struct timeb timebuffer;
	ftime(&timebuffer);
	return timebuffer.time+ ((double)timebuffer.millitm)/1000;
}
int die(string msg, int code) { cerr << msg << code << endl; return code; }

int main() {
    int err = 0;
    cerr << "initializing" << endl;
    if ((err = libusb_init(NULL)) < 0)
        return die("initialize fail ", err);
    cerr << "opening device" << endl;
    libusb_device_handle *dh = libusb_open_device_with_vid_pid(NULL, 0x2ca3, 0x1f);
    if (!dh) return die("open device fail ", 1);

    cerr << "claiming interface" << endl;
    if ((err = libusb_claim_interface(dh, 3)) < 0)
        return die("claim interface fail ", err);

    cerr << "sending magic packet" << endl;
    int tx = 0;
    uint8_t data[] = {0x52, 0x4d, 0x56, 0x54};
    if ((err = libusb_bulk_transfer(dh, 0x03, data, 4, &tx, 500)) < 0)
        cerr << "ERROR: No data transmitted to device " << err << endl; //don't exit

    struct ctx {
        double last;
        int bytes;
    };
    ctx myctx = {now(), 0};
    struct libusb_transfer *xfr = libusb_alloc_transfer(0);
    vector<uint8_t> buf(512);
    libusb_fill_bulk_transfer(xfr, dh, 0x84, buf.data(), buf.size(), [](struct libusb_transfer *xfer){
        fwrite(xfer->buffer, sizeof(uint8_t), xfer->actual_length, stdout);
        ctx* c = (ctx*) xfer->user_data;
        if ((now() - c->last) > (c->bytes? 0.1 : 2)) {
            cerr << "rx " << (c->bytes / (now() - c->last))/1000 << " kb/s" << endl;
            c->last = now();
            c->bytes = 0;
        }
        c->bytes += xfer->actual_length;
        int err;
        if ((err = libusb_submit_transfer(xfer)) < 0) //do another read
            exit(die("read init fail ", err));
    }, &myctx, 100);
    if ((err = libusb_submit_transfer(xfr)) < 0)
        return die("read init fail ", err);

    while(1) {
        if (libusb_handle_events(NULL) != LIBUSB_SUCCESS) break;
    }
    cerr << "closing" << endl;
    libusb_exit(NULL);
    return 0;
}
