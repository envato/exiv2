require 'exiv2'

describe Exiv2 do
  it "should read IPTC data" do
    image = Exiv2::ImageFactory.open("spec/files/test.jpg")
    image.read_metadata
    image.iptc_data.should == {
      "Iptc.Application2.Caption" => "Rhubarb rhubarb rhubard"
    }
  end

  it "should raise an error when trying to open a non-existant file" do
    expect {
      Exiv2::ImageFactory.open("tmp/no-such-file.jpg")
    }.should raise_error(Exiv2::BasicError)
  end
end
