#
#    

CXX    = CC

SRCS   = $(wildcard *.$(CXXEXT))
OBJS   = TFWindow.o glUE.o TLUT.o gluvv.o HSLPicker.o Trackball.o gluvvPrimitive.o sampleRenderer.o MetaVolume.o VolumeRenderable.o VolumeRenderer.o


TARGET      =  vanilla


# Libraries

LIBDIR      = /usr/local/graphics/lib32
LIBS        = -L$(LIBDIR) -lglut -lGLU -lGL -lXmu -lXext -lX11 -lm
INC         = -I/usr/local/graphics/include


.cpp.o: $<
	$(CXX) -c $(INC) $<

$(TARGET): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS) $(INC)
#	$(CP) -f $@ $(INSTBINDIR)

.PHONY: clean clobber
clean:;
	-$(RM) -f $(OBJS) $(TARGET)
	-$(RM) -rf ii_files 

clobber:
	-$(RM) -f $(OBJS) $(TARGET)
	-$(RM) -rf ii_files 
	-$(RM) -rf .depend
	-$(RM) -f *~
	-$(RM) -f *.flc
	-$(RM) -rf $(TARGET)

# EOF 

