clear; close all;
warning('off','all')
Computer = input('Please enter your name\n','s');
TestNum = input('Enter the number of times that you have run this test (eg. "1", "2", etc.)\n');

RadioOrder = randperm(12);
findTx = find(RadioOrder == 7);
for num = 1:11
    if (num < findTx)
        RadioOrder(num) = RadioOrder(num);
    else
        RadioOrder(num) = RadioOrder(num+1);
    end
end
RadioOrder = RadioOrder(1:11);
FixedGainFirst = randi(2,1,11);

radios = 1;
while radios ~= 12
    Radio = string(RadioOrder(radios));
    beep;
    eval(sprintf("str = input('Press Enter when you have switched to Radio Number %s','s');",Radio));
    if (FixedGainFirst(radios) == 1)
        ManualGainFirst = true;
    else
        ManualGainFirst = false;
    end
    
    for runsPerRadio = 1:2
        clear rx;
        plutoradiosetup();
        rx = sdrrx('Pluto');
        rx.RadioID = 'usb:0';
        rx.CenterFrequency = 1250e6;
        rx.BasebandSampleRate = 20e6;
        rx.SamplesPerFrame = 32*64;
        rx.OutputDataType = 'double';
        rx.ShowAdvancedProperties = true;
        if ((ManualGainFirst && (runsPerRadio == 1)) || (~ManualGainFirst && (runsPerRadio == 2)))
            rx.GainSource = 'Manual';
            rx.Gain = 20;
            gainType = 'Manual';
        else
            rx.GainSource = 'AGC Slow Attack';
            gainType = 'Auto';
        end
        testname = 'Radio' + Radio + '_' + Computer + gainType + string(TestNum);
        buffer = zeros(2048,1);
        for clearbuffer = 1:7
            buffer(:,1) = rx();
        end
        for frames = 1:200
            temp(:,frames) = rx();
        end
        eval(sprintf('%s = temp;',testname));
        eval(sprintf('save Tests/%s.mat %s;', testname, testname));
    end
    radios = radios + 1;
end