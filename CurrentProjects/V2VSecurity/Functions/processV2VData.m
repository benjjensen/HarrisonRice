function processV2VData(inFilename, outFilename)
    % Downsample data from 100 MSamples/s to 20 MSamples/s, shift from 30 MHz to baseband, and convert real-values to I/Q values

    % This file converts the data captured by the receiver and data
    % acquisition card to approximately the same type of data output by 
    % the Pluto radios you used in the hallway.
    %
    % The pluto radio data was I/Q (complex-valued) data sampled at 20
    % Msamples/s. 
    %
    % The Van data is real-valued bandpass data sampled at 100 Msamples/s.
    %
    % So, this file converts the real-valued bandpass data sampled at 100
    % Msamples/s to I/Q (complex-valued) data sampled at 20 Msamples/s.
    %
    % Change the variable inFilename to the name of the data set you wish to
    % convert. Nathan has to do his thing first to make it a .mat file.
    %
    % Change ther variable outFilename to the name of the file where you wish
    % to store the I/Q (complex-valued) data sampled at 20 Msamples/s
    %
    % You can then reuse most of your code to process the data stored in
    % outFilename.


    load('Data/myFilterData.mat'); % h g

    firdownsample = dsp.FIRDecimator(5,g);

    mfr = dsp.MatFileReader(inFilename,'VariableName','data','SamplesPerFrame',10000);
    mfw = dsp.MatFileWriter(outFilename,'VariableName','y');

    while ~isDone(mfr)
        temp = mfr();
        x = (temp - 8192)/8192;
        y = firdownsample(x);
        y(2:2:end) = -y(2:2:end);
        mfw(y);
    end
    release(mfr);
    release(mfw);

end