require 'exiv2'

describe Exiv2 do

  it "should handle a Pathname being passed to open" do
    image = Exiv2::ImageFactory.open(Pathname.new("spec/files/test.jpg").to_s)
    image.read_metadata
    image.iptc_data.to_hash.should_not be_empty
  end

  it "should raise an error when trying to open a non-existant file" do
    expect {
      Exiv2::ImageFactory.open("tmp/no-such-file.jpg")
    }.should raise_error(Exiv2::BasicError)
  end

  let(:image) do
    image = Exiv2::ImageFactory.open("spec/files/test.jpg")
    image.read_metadata
    image
  end

  context "IPTC data" do
    before do
      @iptc_data = image.iptc_data
    end

    it "should read IPTC data" do
      @iptc_data.should be_a(Exiv2::IptcData)
      @iptc_data.inspect.should == '#<Exiv2::IptcData: {"Iptc.Application2.Caption"=>"Rhubarb rhubarb rhubard", "Iptc.Application2.Keywords"=>["fish", "custard"]}>'
      @iptc_data.to_a.should == [
        ["Iptc.Application2.Caption", "Rhubarb rhubarb rhubard"],
        ["Iptc.Application2.Keywords", "fish"],
        ["Iptc.Application2.Keywords", "custard"]
      ]
    end

    it "should convert iptc data into a hash" do
      iptc_hash = @iptc_data.to_hash
      iptc_hash.should be_a(Hash)
      iptc_hash.should == {
        "Iptc.Application2.Caption"  => "Rhubarb rhubarb rhubard",
        "Iptc.Application2.Keywords" => ["fish", "custard"]
      }
    end
  end

  context "XMP data" do
    before do
      @xmp_data = image.xmp_data
    end

    it "should read XMP data" do
      @xmp_data.should be_a(Exiv2::XmpData)
      @xmp_data.inspect.should == '#<Exiv2::XmpData: {"Xmp.dc.description"=>"lang=\"x-default\" This is a description", "Xmp.dc.title"=>"lang=\"x-default\" Pickled"}>'
      @xmp_data.to_a.should == [
        ["Xmp.dc.title", "lang=\"x-default\" Pickled"],
        ["Xmp.dc.description", "lang=\"x-default\" This is a description"]
      ]
    end

    it "should convert xmp data into a hash" do
      xmp_hash = @xmp_data.to_hash
      xmp_hash.should be_a(Hash)
      xmp_hash.should == {
        "Xmp.dc.title"       => "lang=\"x-default\" Pickled",
        "Xmp.dc.description" => "lang=\"x-default\" This is a description"
      }
    end

  end

  context "EXIF data" do
    before do
      @exif_data = image.exif_data
    end

    it "should read Exif data" do
      @exif_data.should be_a(Exiv2::ExifData)
      @exif_data.inspect.should == '#<Exiv2::ExifData: {"Exif.Image.ExifTag"=>"52", "Exif.Image.Software"=>"plasq skitch", "Exif.Photo.ExifVersion"=>"48 50 49 48", "Exif.Photo.PixelXDimension"=>"32", "Exif.Photo.PixelYDimension"=>"32"}>'
      @exif_data.to_a.should == [
        ["Exif.Image.Software",         "plasq skitch"],
        ["Exif.Image.ExifTag",          "52"],
        ["Exif.Photo.ExifVersion",      "48 50 49 48"],
        ["Exif.Photo.PixelXDimension",  "32"],
        ["Exif.Photo.PixelYDimension",  "32"]
      ]
    end

    it "should convert xmp data into a hash" do
      exif_hash = @exif_data.to_hash
      exif_hash.should be_a(Hash)
      exif_hash.should == {
        "Exif.Photo.PixelXDimension" => "32",
        "Exif.Photo.ExifVersion"     => "48 50 49 48",
        "Exif.Image.Software"        => "plasq skitch",
        "Exif.Photo.PixelYDimension" => "32",
        "Exif.Image.ExifTag"         => "52"
      }
    end

  end
end
