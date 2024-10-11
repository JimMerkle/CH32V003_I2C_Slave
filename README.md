# CH32V003_I2C_Slave

Configure the I2C as a Slave device, and then analyze the I2C status register bits as an I2C Master interacts with it.

### I2C Master Interaction with Slave
        
        while(1)
        {
            GPIO_WriteBit(GPIOD,GPIO_Pin_6,Bit_SET); // LED on
            int rc = i2c_write(0x12, "01", 2); // 7-bit address:0x12
            if(I2C_ERROR_SUCCESS != rc) printf("i2c_write() Error %d\r\n",rc);
            rc = i2c_write(0x12, "23", 2); // 7-bit address:0x12
            uint8_t data_byte = 0x00;
            rc = i2c_read(0x12, &data_byte, 1);
            if(I2C_ERROR_SUCCESS == rc)
                printf("Received: 0x%02X\r\n",data_byte);
            GPIO_WriteBit(GPIOD,GPIO_Pin_6,Bit_RESET); // LED off
            Delay_Ms(1000);
    }

### Capture Loop
        
        Using a very tight polling loop, combine the value of two 16-bit status bit registers,
        STAR1 and STAR2 into a 32-bit status event sample.  If the event sample changes from the
        previous reading, record sample with a time-stamp into an array.
        Continue looping until timeout (10 seconds), or until the sample array is full.
        During capture loop, if "AF" bit present, software clears it.
        
        Once the sample data has been collected, generate a report showing how the status bits
        change as the I2C state machine processes multiple I2C transactions.
        
### Capture Report
        
        The report loop annotates each line with the name of the status bit that's active
        The listing below shows two loops of the Master I2C code writing and reading bytes.
        
        The bit pattern, "BUSY RXNE" indicates an I2C master write
        The bit pattern, "TRA BUSY TXE" indicates an I2C master read
        
        I2C Slave mode
        Indx  Time   Event bits  Data   Annotations
        000  0123541  0x020000   ----   BUSY
        001  0123639  0x020002   ----   BUSY ADDR
        002  0123640  0x020000   ----   BUSY
        003  0123733  0x020040   0x30   BUSY RXNE
        004  0123734  0x020000   ----   BUSY
        005  0123826  0x020040   0x31   BUSY RXNE
        006  0123828  0x020000   ----   BUSY
        007  0123839  0x000010   ----   STOPF
        008  0123845  0x020010   ----   BUSY STOPF
        009  0124037  0x020050   0x32   BUSY RXNE STOPF
        010  0124038  0x020010   ----   BUSY STOPF
        011  0124130  0x020050   0x33   BUSY RXNE STOPF
        012  0124132  0x020010   ----   BUSY STOPF
        013  0124143  0x000010   ----   STOPF
        014  0124150  0x020010   ----   BUSY STOPF
        015  0124249  0x060090   ----   TRA BUSY TXE STOPF
        016  0124340  0x060490   ----   TRA BUSY AF TXE STOPF
        017  0124341  0x060090   ----   TRA BUSY TXE STOPF
        018  0124353  0x000010   ----   STOPF
        019  1126800  0x020010   ----   BUSY STOPF
        020  1126898  0x020012   ----   BUSY STOPF ADDR
        021  1126899  0x020010   ----   BUSY STOPF
        022  1126992  0x020050   0x30   BUSY RXNE STOPF
        023  1126993  0x020010   ----   BUSY STOPF
        024  1127086  0x020050   0x31   BUSY RXNE STOPF
        025  1127087  0x020010   ----   BUSY STOPF
        026  1127098  0x000010   ----   STOPF
        027  1127105  0x020010   ----   BUSY STOPF
        028  1127296  0x020050   0x32   BUSY RXNE STOPF
        029  1127297  0x020010   ----   BUSY STOPF
        030  1127390  0x020050   0x33   BUSY RXNE STOPF
        031  1127391  0x020010   ----   BUSY STOPF
        032  1127402  0x000010   ----   STOPF
        033  1127409  0x020010   ----   BUSY STOPF
        034  1127508  0x060090   ----   TRA BUSY TXE STOPF
        035  1127599  0x060490   ----   TRA BUSY AF TXE STOPF
        036  1127600  0x060090   ----   TRA BUSY TXE STOPF
        037  1127612  0x060010   ----   TRA BUSY STOPF
        038  1127613  0x000010   ----   STOPF
        
