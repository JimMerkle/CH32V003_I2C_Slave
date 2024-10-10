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
        
        Using a very tight polling loop, combine the value of two 16-bit status bit registers, STAR1 and STAR2
        into a 32-bit status event sample.  If the event sample changes from the previous reading, record sample
        with a time-stamp into an array.  Continue looping until timeout (10 seconds), or until the sample array is full.
        
        Once the sample data has been collected, generate a report showing how the status bits change as the I2C
        state machine processes multiple I2C transactions.
        
### Capture Report
        
        The report loop annotates each line with the name of the status bit that's active
        The listing below shows two loops of the Master I2C code writing and reading bytes.
        
        The bit pattern, "BUSY RXNE" indicates an I2C write
        The bit pattern, "TRA BUSY TXE" indicates an I2C read
        
        I2C Slave mode
        Indx  Time   Event bits  Data   Annotations
        000  0034995  0x020000   ----   BUSY
        001  0035093  0x020002   ----   BUSY ADDR
        002  0035095  0x020000   ----   BUSY
        003  0035187  0x020040   0x30   BUSY RXNE
        004  0035189  0x020000   ----   BUSY
        005  0035281  0x020040   0x31   BUSY RXNE
        006  0035282  0x020000   ----   BUSY
        007  0035293  0x000010   ----   STOPF
        008  0035300  0x020010   ----   BUSY STOPF
        009  0035492  0x020050   0x32   BUSY RXNE STOPF
        010  0035493  0x020010   ----   BUSY STOPF
        011  0035585  0x020050   0x33   BUSY RXNE STOPF
        012  0035587  0x020010   ----   BUSY STOPF
        013  0035598  0x000010   ----   STOPF
        014  0035605  0x020010   ----   BUSY STOPF
        015  0035703  0x060090   ----   TRA BUSY TXE STOPF
        016  0035795  0x060490   ----   TRA BUSY TXE STOPF
        017  0035808  0x000410   ----   STOPF
        018  1038825  0x020410   ----   BUSY STOPF
        019  1039017  0x020450   0x30   BUSY RXNE STOPF
        020  1039018  0x020410   ----   BUSY STOPF
        021  1039111  0x020450   0x31   BUSY RXNE STOPF
        022  1039112  0x020410   ----   BUSY STOPF
        023  1039124  0x000410   ----   STOPF
        024  1039130  0x020410   ----   BUSY STOPF
        025  1039322  0x020450   0x32   BUSY RXNE STOPF
        026  1039323  0x020410   ----   BUSY STOPF
        027  1039416  0x020450   0x33   BUSY RXNE STOPF
        028  1039417  0x020410   ----   BUSY STOPF
        029  1039428  0x000410   ----   STOPF
        030  1039435  0x020410   ----   BUSY STOPF
        031  1039534  0x060490   ----   TRA BUSY TXE STOPF
        032  1039638  0x000410   ----   STOPF
        
