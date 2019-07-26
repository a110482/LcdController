// AlertSoundEffect.h

#ifndef _ALERTSOUNDEFFECT_h
#define _ALERTSOUNDEFFECT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif




#include <avr/pgmspace.h> //http://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html 
#include <toneAC.h> //plays on pins 9 & 10 for ATmega328 for mega 11 12
//#include "Pindefine.h"

//API
void playSirenSound(byte volume, unsigned int period_us);
void playBeep1();

// ToneAC
extern bool busy_status;
byte sirenVolume = 10; //for ToneAC; from 0-10
const unsigned int SIREN_SLOW = 8500; //us; desired delta time between siren freq updates
const unsigned int SIREN_FAST = 5000; //us
unsigned int sirenPeriod = SIREN_SLOW; //us

void playSirenSound(byte volume = sirenVolume, unsigned int period_us = SIREN_SLOW);
//sine wave lookup table to produce the siren sound
static const unsigned int sineLookupTable[] PROGMEM = 
{ 0, 5, 10, 16, 21, 26, 32, 37, 42, 48, 53, 58, 64, 69, 74, 80, 85, 90, 96, 101, 106, 112, 117, 122, 128, 133, 138, 144, 149, 154, 160, 165, 170, 176, 181, 186, 192, 197, 202, 208, 213, 218, 224, 229, 234, 240, 245, 250, 256, 261, 266, 272, 277, 282, 288, 293, 298, 304, 309, 314, 320, 325, 330, 336, 341, 346, 352, 357, 362, 368, 373, 378, 384, 389, 394, 400, 405, 410, 416, 421, 426, 432, 437, 442, 448, 453, 458, 464, 469, 474, 480, 485, 490, 496, 501, 506, 512, 517, 522, 528, 533, 538, 544, 549, 554, 560, 565, 570, 576, 581, 586, 592, 597, 602, 608, 613, 618, 624, 629, 634, 640, 645, 650, 656, 661, 666, 672, 677, 682, 688, 693, 698, 704, 709, 714, 720, 725, 730, 736, 741, 746, 752, 757, 762, 768, 773, 778, 784, 789, 794, 800, 797, 795, 793, 790, 788, 786, 784, 781, 779, 777, 774, 772, 770, 768, 765, 763, 761, 758, 756, 754, 752, 749, 747, 745, 742, 740, 738, 736, 733, 731, 729, 726, 724, 722, 720, 717, 715, 713, 710, 708, 706, 704, 701, 699, 697, 694, 692, 690, 688, 685, 683, 681, 678, 676, 674, 672, 669, 667, 665, 662, 660, 658, 656, 653, 651, 649, 646, 644, 642, 640, 637, 635, 633, 630, 628, 626, 624, 621, 619, 617, 614, 612, 610, 608, 605, 603, 601, 598, 596, 594, 592, 589, 587, 585, 582, 580, 578, 576, 573, 571, 569, 566, 564, 562, 560, 557, 555, 553, 550, 548, 546, 544, 541, 539, 537, 534, 532, 530, 528, 525, 523, 521, 518, 516, 514, 512, 509, 507, 505, 502, 500, 498, 496, 493, 491, 489, 486, 484, 482, 480, 477, 475, 473, 470, 468, 466, 464, 461, 459, 457, 454, 452, 450, 448, 445, 443, 441, 438, 436, 434, 432, 429, 427, 425, 422, 420, 418, 416, 413, 411, 409, 406, 404, 402, 400, 397, 395, 393, 390, 388, 386, 384, 381, 379, 377, 374, 372, 370, 368, 365, 363, 361, 358, 356, 354, 352, 349, 347, 345, 342, 340, 338, 336, 333, 331, 329, 326, 324, 322, 320, 317, 315, 313, 310, 308, 306, 304, 301, 299, 297, 294, 292, 290, 288, 285, 283, 281, 278, 276, 274, 272, 269, 267, 265, 262, 260, 258, 256, 253, 251, 249, 246, 244, 242, 240, 237, 235, 233, 230, 228, 226, 224, 221, 219, 217, 214, 212, 210, 208, 205, 203, 201, 198, 196, 194, 192, 189, 187, 185, 182, 180, 178, 176, 173, 171, 169, 166, 164, 162, 160, 157, 155, 153, 150, 148, 146, 144, 141, 139, 137, 134, 132, 130, 128, 125, 123, 121, 118, 116, 114, 112, 109, 107, 105, 102, 100, 98, 96, 93, 91, 89, 86, 84, 82, 80, 77, 75, 73, 70, 68, 66, 64, 61, 59, 57, 54, 52, 50, 48, 45, 43, 41, 38, 36, 34, 32, 29, 27, 25, 22, 20, 18, 16, 13, 11, 9, 6, 4, 2 };


const unsigned int NUM_SINEWAVE_ELEMENTS = sizeof(sineLookupTable) / sizeof(unsigned int);
unsigned long t_now = 0;
int speed1 = 0, step1 = 0;

void playSirenSound(byte volume, unsigned int period_us) {
	// 啟動條件
	// (digitalRead(ALERT) == LOW )
	if (busy_status) {
		noToneAC();
		return;
	}

	static unsigned long t_start = micros(); //us
	unsigned long t_now = micros(); //us
	if (t_now - t_start >= period_us)
	{
		static int cc = 0;
		int stride = (t_now - t_start) / period_us;		// ------補正頻率
		  
		t_start = t_now; //us; update
		static unsigned int sirenIndex = 0;
		sirenIndex += stride > 1 ? stride - 1 : 0;		// -------補正頻率
		unsigned int freq = pgm_read_word(sineLookupTable + sirenIndex);
		sirenIndex++;
		if (sirenIndex >= NUM_SINEWAVE_ELEMENTS)
			sirenIndex = 0; //start back at beginning of sine wave 
		toneAC(freq + 650, volume);
	}
}

void playBeep1() {
	//local vars
	static unsigned long t_start1 = micros(); //us
	if (t_now - t_start1 >= 5000)
	{
		t_start1 = t_now; //us; update
		speed1 = speed1 + step1;
		if ((speed1 == 0) || (speed1 == 200)) {
			step1 = -step1;
		}
		toneAC(550 + speed1);
	}
}
#endif