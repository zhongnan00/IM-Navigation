/**
 * @file TestNdiSampleApp.cpp
 * @author chao.liu (chao.liu2@medtronic.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include "gtest/gtest.h"
#include "../service/NdiSampleApp.h"

namespace ut{

    class TestNdiSampleApp : public testing::Test
    {

    public:

        void SetUp() override{

        }

        void TearDown() override{

        }


    public:
        app::NdiSampleApp ndiapp;

    };


    TEST_F(TestNdiSampleApp, printHelp)
    {
        std::cout<<"printHelpdd"<<std::endl;
        ndiapp.printHelp();
    }
}



