// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.

// This sample illustrates how to log EMG and IMU data. EMG streaming is only supported for one Myo at a time, and this entire sample is geared to one armband
#include <stdio.h>

#include <windows.h> 
#include <mmsystem.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <time.h>
#include <conio.h>

#include <myo/myo.hpp>

#define VK_ESC		27	// Esc

using namespace std;

//タイムスタンプ取得関数
/*//タイムスタンプ取得部
long long now = unix_timestamp();
std::cout << now;
*/
long long unix_timestamp() {
	long long unix_epoch_from_1601_int64 = 116444736000000000LL;

	//現在時刻をFILETIME構造体に格納
	FILETIME current_ft_from_1601;
	GetSystemTimeAsFileTime(&current_ft_from_1601);

	//現在時刻を格納したFILETIME構造体を64ビット整数型に変換
	ULARGE_INTEGER current_from_1601;
	current_from_1601.HighPart = current_ft_from_1601.dwHighDateTime;
	current_from_1601.LowPart = current_ft_from_1601.dwLowDateTime;

	//現在時刻ファイル時間からUnixエポックファイル時間を引き算
	//→Unixエポックからの経過時間(100ナノ秒単位)へ変換
	long long diff = current_from_1601.QuadPart - unix_epoch_from_1601_int64;

	//10で割って経過時間(100ナノ秒単位)を経過時間(マイクロ秒単位)へ変換
	long long t = diff / 10;

	return t;
}

//2台計測対応，1台でもそのまま動くが余計なログデータを吐くので各自廃棄されたし
class DataCollector : public myo::DeviceListener {
public:
	DataCollector()
	{
		openFiles1();
		openFiles2();
	}

	std::array<int8_t, 8> emgdata;

	void onDisconnect(myo::Myo* myo, uint64_t timestamp) {
		std::cout << "Myo " << identifyMyo(myo) << " has disconnected." << std::endl;
	}
	
	void onPair(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion) {
		knownMyos.push_back(myo);
		//myo->setStreamEmg(myo::Myo::streamEmgEnabled);
		myo->vibrate(myo::Myo::vibrationShort);
		myo->lock();
		emgdata.fill(0);
	}

	size_t identifyMyo(myo::Myo* myo) {
		for (size_t i = 0; i < knownMyos.size(); ++i) {
			if (knownMyos[i] == myo) { return i + 1; }
		}
		return 0;
	}
	std::vector<myo::Myo*> knownMyos;

	void openFiles1() {
		time_t timestamp = std::time(0);

		// Open file for EMG log
		if (emgFile1.is_open()) {
			emgFile1.close();
		}
		std::ostringstream emgFileString1;
		emgFileString1 << "data/emg1.csv";//emgFileString << "data/emg-" << timestamp << ".csv";
		emgFile1.open(emgFileString1.str(), std::ios::out);
		emgFile1 << "timestamp,emg1,emg2,emg3,emg4,emg5,emg6,emg7,emg8" << std::endl;

		// Open file for gyroscope log
		if (gyroFile1.is_open()) {
			gyroFile1.close();
		}
		std::ostringstream gyroFileString1;
		//gyroFileString << "data/gyro-" << timestamp << ".csv";
		gyroFileString1 << "data/gyro1.csv";
		gyroFile1.open(gyroFileString1.str(), std::ios::out);
		gyroFile1 << "timestamp,x,y,z" << std::endl;

		// Open file for accelerometer log
		if (accelerometerFile1.is_open()) {
			accelerometerFile1.close();
		}
		std::ostringstream accelerometerFileString1;
		//accelerometerFileString << "data/accelerometer-" << timestamp << ".csv";
		accelerometerFileString1 << "data/accelerometer1.csv";
		accelerometerFile1.open(accelerometerFileString1.str(), std::ios::out);
		accelerometerFile1 << "timestamp,x,y,z" << std::endl;

		// Open file for orientation log
		if (orientationFile1.is_open()) {
			orientationFile1.close();
		}
		std::ostringstream orientationFileString1;
		//orientationFileString << "data/orientation-" << timestamp << ".csv";
		orientationFileString1 << "data/orientation1.csv";
		orientationFile1.open(orientationFileString1.str(), std::ios::out);
		orientationFile1 << "timestamp,x,y,z,w" << std::endl;

		// Open file for orientation (Euler angles) log
		if (orientationEulerFile1.is_open()) {
			orientationEulerFile1.close();
		}
		std::ostringstream orientationEulerFileString1;
		//orientationEulerFileString << "data/orientationEuler-" << timestamp << ".csv";
		orientationEulerFileString1 << "data/orientationEuler1.csv";
		orientationEulerFile1.open(orientationEulerFileString1.str(), std::ios::out);
		orientationEulerFile1 << "timestamp,roll,pitch,yaw" << std::endl;

	}

	void openFiles2() {
		time_t timestamp = std::time(0);

		// Open file for EMG log
		if (emgFile2.is_open()) {
			emgFile2.close();
		}
		std::ostringstream emgFileString2;
		emgFileString2 << "data/emg2.csv";//emgFileString << "data/emg-" << timestamp << ".csv";
		emgFile2.open(emgFileString2.str(), std::ios::out);
		emgFile2 << "timestamp,emg1,emg2,emg3,emg4,emg5,emg6,emg7,emg8, NO" << std::endl;

		// Open file for gyroscope log
		if (gyroFile2.is_open()) {
			gyroFile2.close();
		}
		std::ostringstream gyroFileString2;
		//gyroFileString << "data/gyro-" << timestamp << ".csv";
		gyroFileString2 << "data/gyro2.csv";
		gyroFile2.open(gyroFileString2.str(), std::ios::out);
		gyroFile2 << "timestamp,x,y,z" << std::endl;

		// Open file for accelerometer log
		if (accelerometerFile2.is_open()) {
			accelerometerFile2.close();
		}
		std::ostringstream accelerometerFileString2;
		//accelerometerFileString << "data/accelerometer-" << timestamp << ".csv";
		accelerometerFileString2 << "data/accelerometer2.csv";
		accelerometerFile2.open(accelerometerFileString2.str(), std::ios::out);
		accelerometerFile2 << "timestamp,x,y,z" << std::endl;

		// Open file for orientation log
		if (orientationFile2.is_open()) {
			orientationFile2.close();
		}
		std::ostringstream orientationFileString2;
		//orientationFileString << "data/orientation-" << timestamp << ".csv";
		orientationFileString2 << "data/orientation2.csv";
		orientationFile2.open(orientationFileString2.str(), std::ios::out);
		orientationFile2 << "timestamp,x,y,z,w" << std::endl;

		// Open file for orientation (Euler angles) log
		if (orientationEulerFile2.is_open()) {
			orientationEulerFile2.close();
		}
		std::ostringstream orientationEulerFileString2;
		//orientationEulerFileString << "data/orientationEuler-" << timestamp << ".csv";
		orientationEulerFileString2 << "data/orientationEuler2.csv";
		orientationEulerFile2.open(orientationEulerFileString2.str(), std::ios::out);
		orientationEulerFile2 << "timestamp,roll,pitch,yaw" << std::endl;

	}

	// onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
	void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
	{
		printf("\r");
		if (identifyMyo(myo) == 0) {
			emgFile1 << timestamp;//now;
			for (size_t i = 0; i < 8; i++) {
				emgFile1 << ',' << static_cast<int>(emg[i]);
				printf("[%d] ", emg[i]);
			}
			emgFile1 << std::endl; //emgFile1 << "," << identifyMyo(myo) << std::endl;
		}
		else if (identifyMyo(myo) == 1) {
			emgFile2 << timestamp;//now;
			for (size_t i = 0; i < 8; i++) {
				emgFile2 << ',' << static_cast<int>(emg[i]);
				//printf("[%d] ", emg[i]);
			}
			emgFile2 << std::endl; //emgFile2 << "," << identifyMyo(myo) << std::endl;
		}

	}

	// onOrientationData is called whenever new orientation data is provided
	// Be warned: This will not make any distiction between data from other Myo armbands
	void onOrientationData(myo::Myo *myo, uint64_t timestamp, const myo::Quaternion< float > &rotation) {
		if (identifyMyo(myo) == 0) {
			orientationFile1 << timestamp
				<< ',' << rotation.x()
				<< ',' << rotation.y()
				<< ',' << rotation.z()
				<< ',' << rotation.w()
				<< std::endl;
		}
		else if (identifyMyo(myo) == 1) {
			orientationFile2 << timestamp
				<< ',' << rotation.x()
				<< ',' << rotation.y()
				<< ',' << rotation.z()
				<< ',' << rotation.w()
				<< std::endl;
		}

		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::max;
		using std::min;

		// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
		float roll = atan2(2.0f * (rotation.w() * rotation.x() + rotation.y() * rotation.z()),
			1.0f - 2.0f * (rotation.x() * rotation.x() + rotation.y() * rotation.y()));
		float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (rotation.w() * rotation.y() - rotation.z() * rotation.x()))));
		float yaw = atan2(2.0f * (rotation.w() * rotation.z() + rotation.x() * rotation.y()),
			1.0f - 2.0f * (rotation.y() * rotation.y() + rotation.z() * rotation.z()));

		if (identifyMyo(myo) == 0) {
			orientationEulerFile1 << timestamp
				<< ',' << roll
				<< ',' << pitch
				<< ',' << yaw
				<< std::endl;
		}
		else if (identifyMyo(myo) == 1) {
			orientationEulerFile2 << timestamp
				<< ',' << roll
				<< ',' << pitch
				<< ',' << yaw
				<< std::endl;
		}
	}

	// onAccelerometerData is called whenever new acceleromenter data is provided
	// Be warned: This will not make any distiction between data from other Myo armbands
	void onAccelerometerData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &accel) {
		if (identifyMyo(myo) == 0) {
			printVector(accelerometerFile1, timestamp, accel);
		}
		else if (identifyMyo(myo) == 1) {
			printVector(accelerometerFile2, timestamp, accel);
		}

	}

	// onGyroscopeData is called whenever new gyroscope data is provided
	// Be warned: This will not make any distiction between data from other Myo armbands
	void onGyroscopeData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &gyro) {
		if (identifyMyo(myo) == 0) {
			printVector(gyroFile1, timestamp, gyro);
		}
		else if (identifyMyo(myo) == 1) {
			printVector(gyroFile2, timestamp, gyro);
		}

	}

	void onConnect(myo::Myo *myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion) {
		//Reneable streaming
		myo->setStreamEmg(myo::Myo::streamEmgEnabled);
		if (identifyMyo(myo) == 0) {
			openFiles1();
		}
		else if (identifyMyo(myo) == 1) {
			openFiles2();
		}
		
		std::cout << "Myo " << identifyMyo(myo) << " has connected." << std::endl;
	}

	// Helper to print out accelerometer and gyroscope vectors
	void printVector(std::ofstream &file, uint64_t timestamp, const myo::Vector3< float > &vector) {
		file << timestamp
			<< ',' << vector.x()
			<< ',' << vector.y()
			<< ',' << vector.z()
			<< std::endl;
	}

	// The files we are logging to
	std::ofstream emgFile1;
	std::ofstream gyroFile1;
	std::ofstream orientationFile1;
	std::ofstream orientationEulerFile1;
	std::ofstream accelerometerFile1;

	std::ofstream emgFile2;
	std::ofstream gyroFile2;
	std::ofstream orientationFile2;
	std::ofstream orientationEulerFile2;
	std::ofstream accelerometerFile2;

};

int main(int argc, char** argv)
{
	// We catch any exceptions that might occur below -- see the catch statement for more details.
	try {

		// First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
		// publishing your application. The Hub provides access to one or more Myos.
		myo::Hub hub("com.undercoveryeti.myo-data-capture");//"com.undercoveryeti.myo-data-capture"  "com.example.multiple-myos"

		std::cout << "Attempting to find a Myo..." << std::endl;

		// Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
		// immediately.
		// waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
		// if that fails, the function will return a null pointer.
		myo::Myo* myo = hub.waitForMyo(10000);//10秒間MYOが見つからなければNULL返す

		// If waitForMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}

		// We've found a Myo.
		std::cout << "Connected to a Myo armband! Logging to the file system. Check the folder this appliation lives in." << std::endl << std::endl;

		// Next we enable EMG streaming on the found Myo.
		myo->setStreamEmg(myo::Myo::streamEmgEnabled);

		// Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
		DataCollector collector;

		// Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
		// Hub::run() to send events to all registered device listeners.
		hub.addListener(&collector);

		printf("キー操作で計測開始，キー操作で終了\n");
		while (!kbhit()) {}//キー入力されるまで待機

		unsigned char key = getch();
		printf("start\n");
		while (!kbhit()) {
			// In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
			// In this case, we wish to update our display 200 times a second, so we run for 1000/200 milliseconds.
			hub.run(1000 / 200); //サンプリングレート指定，分母の数値で変更可能，1～200Hz
		}

		// If a standard exception occurred, we print out its message and exit.
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}
}