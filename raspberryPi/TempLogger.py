#!/usr/bin/python
# -*- coding: utf-8 -*-

#
#       TempLogger.py
#
#       Copyright 2016 Audrey Robinel - EmNet.cc
#
#       This program is free software; you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation; either version 2 of the License, or
#       (at your option) any later version.
#
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#
#       You should have received a copy of the GNU General Public License
#       along with this program; if not, write to the Free Software
#       Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#       MA 02110-1301, USA.


"""This program reads the temperatures of the attached probes and records
it to make statistics.
"""
#import RPi.GPIO as GPIO
import time
import serial
import datetime
import array
import numpy

#__all__ = ['TempLogger', 'run']

class TempLogger:
    'logging temperature for Rlieh'
    __serial_port_name='/dev/ttyAMA0'
    __serial_baudrate=115200
    __serial_timeout=0.05
    __measures_delay=1
    __last_temps_str=None
    __last_temps=array.array('f',[])
    __temps_probes_count=4
    __temp_probes_labels=['eau1','eau2','eau3','air']
    __temps_count=60
    __temps_array=list()
    __max_temps1=array.array('f',[])
    __min_temps1=array.array('f',[])

    __avg_temps_logfile="/mnt/ramdisk/avg_temp_logs.txt"
    #__min_max_reset_time=3600
    #__temps_avg=

    __turn=0
    __reports_interval=6



    def __init__(self):
        for i in range(0,self.__temps_probes_count):
            self.__last_temps.append(-127.0)
            self.__temps_array.append(array.array('f',[]))
            self.__max_temps1.append(-127)
            self.__min_temps1.append(127)
            self.__reports_interval=self.__temps_count

            self.__turn=0

        #self.__temps_array=numpy.zeros((self.__temps_probes_count, self.__temps_count))
        #self.__last_temps_str=None
        #self.__serial_port_name='/dev/ttyAMA0'
        #self.__serial_baudrate=115200
        #self.__serial_timeout=0.05

    def parse_temps(self):
        if(self.__last_temps_str!='' and self.__last_temps_str!='' and self.__last_temps_str!=' '):
            for i in range(0,self.__temps_probes_count):
                self.__last_temps[i]=float(self.__last_temps_str[i*6:(i+1)*6-1])
                #print self.__last_temps[i]

    def print_last_temps(self):
        for i in range(0,self.__temps_probes_count):
            if(self.__last_temps[i]>-127):
                print("%4s=%.2f°C" % (self.__temp_probes_labels[i],self.__last_temps[i]))

    def print_avg_temps(self):
        print ("report : average temps")
        for i in range(0,self.__temps_probes_count):
            print("%4s=%.2f°C" % (self.__temp_probes_labels[i],self.calc_avg_temp(self.__temps_count,i)))
            #print self.calc_avg_temp(self.__temps_count,i)


    def calc_avg_temp(self,nb_mesures,probe_index):
        sum0=0.0
        for i in range(0,nb_mesures):
            sum0+=self.__temps_array[probe_index][i]
        avg0=sum0/nb_mesures
        return round(avg0,2)

    def search_min_temp(self,nb_mesures,probe_index):
        min_t=127.0
        for i in range(0,nb_mesures):
            if(self.__temps_array[probe_index][i]<min_t):
                min_t=self.__temps_array[probe_index][i]
        return min_t

    def search_max_temp(self,nb_mesures,probe_index):
        max_t=-127.0
        for i in range(0,nb_mesures):
            if(self.__temps_array[probe_index][i]>max_t):
                max_t=self.__temps_array[probe_index][i]
        return max_t





    def log_temps(self):
        while True:
            ser =  serial.Serial(self.__serial_port_name, self.__serial_baudrate, timeout=self.__serial_timeout)
            time.sleep(0.1)
            ser.write("getTC:00")
            time.sleep(0.1)
            self.__last_temps_str = ser.readline()
            self.parse_temps()

            #self.print_last_temps()

            #storing values to compute averages
            for i in range(0,self.__temps_probes_count):
                if(self.__last_temps[i]>-127):
                    self.__temps_array[i].append(self.__last_temps[i])
                if(len(self.__temps_array[i])>self.__temps_count):
                    self.__temps_array[i].pop(0)
                    #print self.calc_avg_temp(self.__temps_count,i)



            #print(self.__last_temps)
            #f = open("/mnt/ramdisk/temp_logs.txt", 'a')
            #f.write(str(time.time()))
            #f.write(';')
            #f.write(temps)
            #f.write('\n')
            time.sleep(self.__measures_delay)
            self.__turn+=1
            #print self.__turn
            if(self.__turn>self.__reports_interval):
                self.__turn=0
                self.print_avg_temps()
                f = open(self.__avg_temps_logfile, 'a')
                f.write(str(int(time.time())))
                f.write(';')
                for i in range(0,self.__temps_probes_count):
                    f.write("%.2f°C;" % (self.calc_avg_temp(self.__temps_count,i)))
                f.write('\n')
                f.close()

                f = open("/home/pi/code/min_t.txt", 'a')
                f.write(str(int(time.time())))
                f.write(';')
                for i in range(0,self.__temps_probes_count):
                    f.write("%.2f°C;" % (self.search_min_temp(self.__temps_count,i)))
                f.write('\n')
                f.close()

                f = open("/home/pi/code/max_t.txt", 'a')
                f.write(str(int(time.time())))
                f.write(';')
                for i in range(0,self.__temps_probes_count):
                    f.write("%.2f°C;" % (self.search_max_temp(self.__temps_count,i)))
                f.write('\n')
                f.close()




    def run(self):
        self.log_temps()



def run():
    action = TempLogger().run()

if __name__ == '__main__':
    run()
