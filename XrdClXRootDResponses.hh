//------------------------------------------------------------------------------
// Copyright (c) 2012 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
// See the LICENCE file for details.
//------------------------------------------------------------------------------

#ifndef __XRD_CL_XROOTD_RESPONSES_HH__
#define __XRD_CL_XROOTD_RESPONSES_HH__

#include "XrdCl/XrdClBuffer.hh"
#include "XrdCl/XrdClStatus.hh"
#include "XrdCl/XrdClAnyObject.hh"
#include "XProtocol/XProtocol.hh"
#include <string>
#include <vector>
#include <list>
#include <ctime>

namespace XrdClient
{
  //----------------------------------------------------------------------------
  //! Path location info
  //----------------------------------------------------------------------------
  class LocationInfo
  {
    public:
      //------------------------------------------------------------------------
      //! Describes the node type and file status for a given location
      //------------------------------------------------------------------------
      enum LocationType
      {
        ManagerOnline,   //!< manager node where the file is online
        ManagerPending,  //!< manager node where the file is pending to be online
        ServerOnline,    //!< server node where the file is online
        ServerPending    //!< server node where the file is pending to be online
      };

      //------------------------------------------------------------------------
      //! Describes the allowed access type for the file at given location
      //------------------------------------------------------------------------
      enum AccessType
      {
        Read,            //!< read access is allowed
        ReadWrite        //!< write access is allowed
      };

      //------------------------------------------------------------------------
      //! Location
      //------------------------------------------------------------------------
      class Location
      {
        public:

          //--------------------------------------------------------------------
          //! Constructor
          //--------------------------------------------------------------------
          Location( const std::string  &address,
                    LocationType        type,
                    AccessType          access ):
            pAddress( address ),
            pType( type ),
            pAccess( access ) {}

          //--------------------------------------------------------------------
          //! Get address
          //--------------------------------------------------------------------
          const std::string &GetAddress() const
          {
            return pAddress;
          }

          //--------------------------------------------------------------------
          //! Get location type
          //--------------------------------------------------------------------
          LocationType GetType() const
          {
            return pType;
          }

          //--------------------------------------------------------------------
          //! Get access type
          //--------------------------------------------------------------------
          AccessType GetAccessType() const
          {
            return pAccess;
          }

          //--------------------------------------------------------------------
          //! Check whether the location is a server
          //--------------------------------------------------------------------
          bool IsServer() const
          {
            return pType == ServerOnline || pType == ServerPending;
          }

          //--------------------------------------------------------------------
          //! Check whether the location is a manager
          //--------------------------------------------------------------------
          bool IsManager() const
          {
            return pType == ManagerOnline || pType == ManagerPending;
          }

        private:
          std::string  pAddress;
          LocationType pType;
          AccessType   pAccess;
      };

      //------------------------------------------------------------------------
      //! List of locations
      //------------------------------------------------------------------------
      typedef std::vector<Location>        LocationList;

      //------------------------------------------------------------------------
      //! Iterator over locations
      //------------------------------------------------------------------------
      typedef LocationList::iterator       Iterator;

      //------------------------------------------------------------------------
      //! Iterator over locations
      //------------------------------------------------------------------------
      typedef LocationList::const_iterator ConstIterator;

      //------------------------------------------------------------------------
      //! Constructor
      //------------------------------------------------------------------------
      LocationInfo( const char *data = 0 );

      //------------------------------------------------------------------------
      //! Get number of locations
      //------------------------------------------------------------------------
      uint32_t GetSize() const
      {
        return pLocations.size();
      }

      //------------------------------------------------------------------------
      //! Get the location at index
      //------------------------------------------------------------------------
      Location &At( uint32_t index )
      {
        return pLocations[index];
      }

      //------------------------------------------------------------------------
      //! Get the location begin iterator
      //------------------------------------------------------------------------
      Iterator Begin()
      {
        return pLocations.begin();
      }

      //------------------------------------------------------------------------
      //! Get the location begin iterator
      //------------------------------------------------------------------------
      ConstIterator Begin() const
      {
        return pLocations.begin();
      }

      //------------------------------------------------------------------------
      //! Get the location end iterator
      //------------------------------------------------------------------------
      Iterator End()
      {
        return pLocations.end();
      }

      //------------------------------------------------------------------------
      //! Get the location end iterator
      //------------------------------------------------------------------------
      ConstIterator End() const
      {
        return pLocations.end();
      }

      //------------------------------------------------------------------------
      //! Add a location
      //------------------------------------------------------------------------
      void Add( const Location &location )
      {
        pLocations.push_back( location );
      }

    private:
      void ParseServerResponse( const char *data );
      void ProcessLocation( std::string &location );
      LocationList pLocations;
  };

  //----------------------------------------------------------------------------
  //! Request status
  //----------------------------------------------------------------------------
  class XRootDStatus: public Status
  {
    public:
      //------------------------------------------------------------------------
      //! Constructor
      //------------------------------------------------------------------------
      XRootDStatus( uint16_t           st      = 0,
                    uint16_t           code    = 0,
                    uint32_t           errN    = 0,
                    const std::string &message = "" ):
        Status( st, code, errN ),
        pMessage( message ) {}

      //------------------------------------------------------------------------
      //! Constructor
      //------------------------------------------------------------------------
      XRootDStatus( const Status      &st,
                    const std::string &message = "" ):
        Status( st ),
        pMessage( message ) {}

      //------------------------------------------------------------------------
      //! Get error message
      //------------------------------------------------------------------------
      const std::string &GetErrorMessage() const
      {
        return pMessage;
      }

      //------------------------------------------------------------------------
      //! Set the error message
      //------------------------------------------------------------------------
      void SetErrorMessage( const std::string &message )
      {
        pMessage = message;
      }

      //------------------------------------------------------------------------
      //! Convert to string
      //------------------------------------------------------------------------
      std::string ToStr() const
      {

        if( code == errErrorResponse )
        {
          std::ostringstream o;
          o << "[ERROR] Server responded with an error: [" << errNo << "] ";
          o << pMessage << std::endl;
          return o.str();
        }
        return ToString();
      }

    private:
      std::string pMessage;
  };

  //----------------------------------------------------------------------------
  //! Binary buffer
  //----------------------------------------------------------------------------
  typedef Buffer BinaryDataInfo;

  //----------------------------------------------------------------------------
  //! Protocol response
  //----------------------------------------------------------------------------
  class ProtocolInfo
  {
    public:
      //------------------------------------------------------------------------
      //! Types of XRootD servers
      //------------------------------------------------------------------------
      enum HostTypes
      {
        IsManager = kXR_isManager,   //!< Manager
        IsServer  = kXR_isServer,    //!< Data server
        AttrMeta  = kXR_attrMeta,    //!< Meta attribute
        AttrProxy = kXR_attrProxy,   //!< Proxy attribute
        AttrSuper = kXR_attrSuper,   //!< Supervisor attribute
      };

      //------------------------------------------------------------------------
      //! Constructor
      //------------------------------------------------------------------------
      ProtocolInfo( uint32_t version, uint32_t hostInfo ):
        pVersion( version ), pHostInfo( hostInfo ) {}

      //------------------------------------------------------------------------
      //! Get version info
      //------------------------------------------------------------------------
      uint32_t GetVersion() const
      {
        return pVersion;
      }

      //------------------------------------------------------------------------
      //! Get host info
      //------------------------------------------------------------------------
      uint32_t GetHostInfo() const
      {
        return pHostInfo;
      }

      //------------------------------------------------------------------------
      //! Test host info flags
      //------------------------------------------------------------------------
      bool TestHostInfo( uint32_t flags )
      {
        return pHostInfo & flags;
      }

    private:
      uint32_t pVersion;
      uint32_t pHostInfo;
  };

  //----------------------------------------------------------------------------
  //! Object stat info
  //----------------------------------------------------------------------------
  class StatInfo
  {
    public:
      //------------------------------------------------------------------------
      //! Flags
      //------------------------------------------------------------------------
      enum Flags
      {
        XBitSet      = kXR_xset,      //!< Executable/searchable bit set
        IsDir        = kXR_isDir,     //!< This is a directory
        Other        = kXR_other,     //!< Neither a file nor a directory
        Offline      = kXR_offline,   //!< File is not online (ie. on disk)
        POSCPending  = kXR_poscpend,  //!< File opened with POST flag, not yet
                                      //!< successfuly closed
        IsReadable   = kXR_readable,  //!< Read access is alowed
        IsWritable   = kXR_writable,  //!< Write access is allowed
      };

      //------------------------------------------------------------------------
      //! Constructor
      //------------------------------------------------------------------------
      StatInfo( const char *data );

      //------------------------------------------------------------------------
      //! Get id
      //------------------------------------------------------------------------
      const std::string GetId() const
      {
        return pId;
      }

      //------------------------------------------------------------------------
      //! Get size (in bytes)
      //------------------------------------------------------------------------
      uint64_t GetSize() const
      {
        return pSize;
      }

      //------------------------------------------------------------------------
      //! Get flags
      //------------------------------------------------------------------------
      uint32_t GetFlags() const
      {
        return pFlags;
      }

      //------------------------------------------------------------------------
      //! Test flags
      //------------------------------------------------------------------------
      bool TestFlags( uint32_t flags ) const
      {
        return pFlags & flags;
      }

      //------------------------------------------------------------------------
      //! Get modification time (in seconds since epoch)
      //------------------------------------------------------------------------
      uint64_t GetModTime() const
      {
        return pModTime;
      }

      //------------------------------------------------------------------------
      //! Get modification time
      //------------------------------------------------------------------------
      std::string GetModTimeAsString() const
      {
        char ts[256];
        time_t modTime = pModTime;
        tm *t = gmtime( &modTime );
        strftime( ts, 255, "%F %T", t );
        return ts;
      }


    private:

      //------------------------------------------------------------------------
      // Parse the stat info returned by the server
      //------------------------------------------------------------------------
      void ParseServerResponse( const char *data  );

      //------------------------------------------------------------------------
      // Normal stat
      //------------------------------------------------------------------------
      std::string pId;
      uint64_t    pSize;
      uint32_t    pFlags;
      uint64_t    pModTime;
  };

  //----------------------------------------------------------------------------
  //! VFS stat info
  //----------------------------------------------------------------------------
  class StatInfoVFS
  {
    public:
      //------------------------------------------------------------------------
      //! Constructor
      //------------------------------------------------------------------------
      StatInfoVFS( const char *data );

      //------------------------------------------------------------------------
      //! Get number of nodes that can provide read/write space
      //------------------------------------------------------------------------
      uint64_t GetNodesRW() const
      {
        return pNodesRW;
      }

      //------------------------------------------------------------------------
      //! Get size of the largest contiguous aread of free r/w space (in MB)
      //------------------------------------------------------------------------
      uint64_t GetFreeRW() const
      {
        return pFreeRW;
      }

      //------------------------------------------------------------------------
      //! Get percentage of the partition utilization represented by FreeRW
      //------------------------------------------------------------------------
      uint8_t GetUtilizationRW() const
      {
        return pUtilizationRW;
      }

      //------------------------------------------------------------------------
      //! Get number of nodes that can provide staging space
      //------------------------------------------------------------------------
      uint64_t GetNodesStaging() const
      {
        return pNodesStaging;
      }

      //------------------------------------------------------------------------
      //! Get size of the largest contiguous aread of free staging space (in MB)
      //------------------------------------------------------------------------
      uint64_t GetFreeStaging() const
      {
        return pFreeStaging;
      }

      //------------------------------------------------------------------------
      //! Get percentage of the partition utilization represented by FreeStaging
      //------------------------------------------------------------------------
      uint8_t GetUtilizationStaging() const
      {
        return pUtilizationStaging;
      }

    private:

      //------------------------------------------------------------------------
      // Parse the stat info returned by the server
      //------------------------------------------------------------------------
      void ParseServerResponse( const char *data  );

      //------------------------------------------------------------------------
      // kXR_vfs stat
      //------------------------------------------------------------------------
      uint64_t    pNodesRW;
      uint64_t    pFreeRW;
      uint32_t    pUtilizationRW;
      uint64_t    pNodesStaging;
      uint64_t    pFreeStaging;
      uint32_t    pUtilizationStaging;
  };

  //----------------------------------------------------------------------------
  //! Directory list
  //----------------------------------------------------------------------------
  class DirectoryList
  {
    public:
      //------------------------------------------------------------------------
      //! Directory entry
      //------------------------------------------------------------------------
      class ListEntry
      {
        public:
          //--------------------------------------------------------------------
          //! Constructor
          //--------------------------------------------------------------------
          ListEntry( const std::string &hostAddress,
                     const std::string &name,
                     StatInfo          *statInfo = 0):
            pHostAddress( hostAddress ),
            pName( name ),
            pStatInfo( statInfo )
          {}

          //--------------------------------------------------------------------
          //! Destructor
          //--------------------------------------------------------------------
          ~ListEntry()
          {
            delete pStatInfo;
          }

          //--------------------------------------------------------------------
          //! Get host address
          //--------------------------------------------------------------------
          const std::string &GetHostAddress() const
          {
            return pHostAddress;
          }

          //--------------------------------------------------------------------
          //! Get file name
          //--------------------------------------------------------------------
          const std::string &GetName() const
          {
            return pName;
          }

          //--------------------------------------------------------------------
          //! Get the stat info object
          //--------------------------------------------------------------------
          StatInfo *GetStatInfo()
          {
            return pStatInfo;
          }

          //--------------------------------------------------------------------
          //! Get the stat info object
          //--------------------------------------------------------------------
          const StatInfo *GetStatInfo() const
          {
            return pStatInfo;
          }

          //--------------------------------------------------------------------
          //! Set the stat info object (and transfer the ownership)
          //--------------------------------------------------------------------
          void SetStatInfo( StatInfo *info )
          {
            pStatInfo = info;
          }

        private:
          std::string  pHostAddress;
          std::string  pName;
          StatInfo    *pStatInfo;
      };

      //------------------------------------------------------------------------
      //! Constructor
      //------------------------------------------------------------------------
      DirectoryList( const std::string &hostID,
                     const std::string &parent,
                     const char        *data );

      //------------------------------------------------------------------------
      //! Destructor
      //------------------------------------------------------------------------
      ~DirectoryList();

      //------------------------------------------------------------------------
      //! Directory listing
      //------------------------------------------------------------------------
      typedef std::vector<ListEntry*>  DirList;

      //------------------------------------------------------------------------
      //! Directory listing iterator
      //------------------------------------------------------------------------
      typedef DirList::iterator       Iterator;

      //------------------------------------------------------------------------
      //! Directory listing const iterator
      //------------------------------------------------------------------------
      typedef DirList::const_iterator ConstIterator;

      //------------------------------------------------------------------------
      //! Add an entry to the list - takes ownership
      //------------------------------------------------------------------------
      void Add( ListEntry *entry )
      {
        pDirList.push_back( entry );
      }

      //------------------------------------------------------------------------
      //! Get an entry at given index
      //------------------------------------------------------------------------
      ListEntry *At( uint32_t index )
      {
        return pDirList[index];
      }

      //------------------------------------------------------------------------
      //! Get the begin iterator
      //------------------------------------------------------------------------
      Iterator Begin()
      {
        return pDirList.begin();
      }

      //------------------------------------------------------------------------
      //! Get the begin iterator
      //------------------------------------------------------------------------
      ConstIterator Begin() const
      {
        return pDirList.end();
      }

      //------------------------------------------------------------------------
      //! Get the end iterator
      //------------------------------------------------------------------------
      Iterator End()
      {
        return pDirList.end();
      }

      //------------------------------------------------------------------------
      //! Get the end iterator
      //------------------------------------------------------------------------
      ConstIterator End() const
      {
        return pDirList.end();
      }

      //------------------------------------------------------------------------
      //! Get the size of the listing
      //------------------------------------------------------------------------
      uint32_t GetSize() const
      {
        return pDirList.size();
      }

      //------------------------------------------------------------------------
      //! Get parent directory name
      //------------------------------------------------------------------------
      const std::string &GetParentName() const
      {
        return pParent;
      }

    private:
      void ParseServerResponse( const std::string &hostId, const char *data );
      DirList     pDirList;
      std::string pParent;
  };

  //----------------------------------------------------------------------------
  //! Handle an async response
  //----------------------------------------------------------------------------
  class ResponseHandler
  {
    public:
      //------------------------------------------------------------------------
      //! Called when a response to associated request arrives or an error
      //! occurs
      //!
      //! @param status   status of the request
      //! @param response an object associated with the response
      //!                 (request dependent)
      //------------------------------------------------------------------------
      virtual void HandleResponse( XRootDStatus *status,
                                   AnyObject    *response ) = 0;
  };
}

#endif // __XRD_CL_XROOTD_RESPONSES_HH__
